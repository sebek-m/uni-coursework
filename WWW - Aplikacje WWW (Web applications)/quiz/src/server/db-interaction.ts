import * as sqlite3 from 'sqlite3';
import { Player } from './player';
import { Response, UserScore, QuizMeta, QuestionBase, QuizBase, AnonymousQuizBase, ResponseRaw } from '../interfaces'

class QuizDB {
  private readonly dbName: string;

  constructor(name: string) {
    this.dbName = name;
  }

  get name(): string {
    return this.dbName;
  }

  public async initDB() {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let creationProm = new Promise<QuizDB>((resolve, reject) => {
      db.serialize(() => {
        db.run('DROP TABLE IF EXISTS response;')
          .run('DROP TABLE IF EXISTS question;')
          .run('DROP TABLE IF EXISTS quiz;')
          .run('DROP TABLE IF EXISTS player;')
          .run(`CREATE TABLE IF NOT EXISTS player (
                  id INTEGER PRIMARY KEY NOT NULL,
                  username TEXT NOT NULL UNIQUE,
                  password TEXT NOT NULL
                );`)
          .run(`CREATE TABLE IF NOT EXISTS quiz (
                  id INTEGER PRIMARY KEY NOT NULL,
                  title TEXT NOT NULL UNIQUE,
                  description TEXT NOT NULL
                );`)
          .run(`CREATE TABLE IF NOT EXISTS question (
                  id INTEGER PRIMARY KEY NOT NULL,
                  quiz_id INTEGER NOT NULL,
                  question_num INTEGER NOT NULL,
                  content TEXT NOT NULL,
                  correct_answer TEXT NOT NULL,
                  wrong_ans_penalty INTEGER NOT NULL,
                  UNIQUE(quiz_id, content),
                  UNIQUE(quiz_id, question_num)
                );`)
          .run(`CREATE TABLE IF NOT EXISTS response (
                  question_id INTEGER NOT NULL,
                  player_id INTEGER NOT NULL,
                  answer TEXT NOT NULL,
                  answer_time INTEGER NOT NULL,
                  correct INTEGER NOT NULL,
                  PRIMARY KEY(question_id, player_id)
                );`, [], (err) => {
          if (err)
            return reject(err);
          
          resolve();
        });
      });
    });

    await creationProm;

    db.close();
  }

  public async addPlayer(username: string, password: string) {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let insertionProm = new Promise((resolve, reject) => {
      db.run('INSERT OR IGNORE INTO player (username, password) VALUES (?, ?);',
              [username, password], (err) => {
        if (err)
          return reject(err);

        resolve();
      });
    });

    db.close();

    await insertionProm;
  }

  public async addQuiz(quizBase: AnonymousQuizBase) {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let insertionProm = new Promise((resolve, reject) => {
      db.run('INSERT INTO quiz (title, description) VALUES (?, ?);',
              [quizBase.meta.title, quizBase.meta.description], function(err) {
        if (err)
          return reject(err);

        let questionsForInsertion: (string | number)[] = [];
        for (const [num, question] of quizBase.questions.entries()) {
          questionsForInsertion = questionsForInsertion.concat([
            this.lastID, // Kolumna quiz_id
            num, // Kolumna question_num
            question.content, // Kolumna content
            question.correct_answer, // Kolumna correct_answer
            question.wrong_ans_penalty // Kolumna wrong_ans_penalty
          ]);
        }

        let placeholders = quizBase.questions.map((question) => '(?, ?, ?, ?, ?)').join(',');
        let sql = 'INSERT INTO question (quiz_id, question_num, content, correct_answer, wrong_ans_penalty) VALUES ' + placeholders;

        db.run(sql, questionsForInsertion, (err) => {
          if (err)
            return reject(err);

          resolve();
        });

        db.close();
      });
    });

    await insertionProm;
  }

  public async getPlayer(identifier: string | number, colName: string): Promise<Player | false> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let queryProm = new Promise<Player | false>((resolve, reject) => {
      db.get(`SELECT * FROM player WHERE ${colName} = ?;`, identifier, (err, row) => {
        if (err)
          return reject(err);

        if (!row)
          return resolve(false);

        resolve(new Player(row.id, row.username, row.password));
      });
    });

    db.close();

    return queryProm;
  }

  public async getQuizMeta(quizID: number): Promise<QuizMeta> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let queryProm = new Promise<QuizMeta>((resolve, reject) => {
      db.get(`SELECT * FROM quiz
              WHERE id = ?;`, quizID, (err, quiz) => {
        if (err)
          return reject(err);

        resolve(quiz as QuizMeta);
      });
    });

    db.close();

    return queryProm;
  }
  
  public async getUnsolvedQuizCount(player: Player): Promise<number> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let queryProm = new Promise<number>((resolve, reject) => {
      db.get(`SELECT COUNT(*) AS quiz_count FROM quiz
              WHERE id NOT IN (
                SELECT quiz_id FROM question
                WHERE id IN (
                  SELECT question_id FROM response
                  WHERE player_id = ?
                )
              ) ORDER BY title ASC;`, player.id, (err, quiz_count) => {
        if (err)
          return reject(err);

        resolve(quiz_count.quiz_count);
      });
    });

    db.close();

    return queryProm;
  }

  public async getQuizList(player: Player, solved: boolean): Promise<QuizMeta[]> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let is_in = solved ? 'IN' : 'NOT IN';

    let queryProm = new Promise<QuizMeta[]>((resolve, reject) => {
      db.all(`SELECT * FROM quiz
              WHERE id ${is_in} (
                SELECT quiz_id FROM question
                WHERE id IN (
                  SELECT question_id FROM response
                  WHERE player_id = ?
                )
              ) ORDER BY title ASC;`, player.id, (err, quizzes) => {
        if (err)
          return reject(err);

        resolve(quizzes as QuizMeta[]);
      });
    });

    db.close();

    return queryProm;
  }

  public async getQuestions(quizID: number): Promise<QuestionBase[]> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let queryProm = new Promise<QuestionBase[]>((resolve, reject) => {
      db.all(`SELECT id, content, correct_answer, wrong_ans_penalty
              FROM question
              WHERE quiz_id = ?
              ORDER BY question_num ASC;`, quizID, (err, questions) => {
        if (err)
          return reject(err);

        if (!questions)
          return reject('Niespodziewana niespójność w bazie danych - quiz nie ma pytań');

        return resolve(questions as QuestionBase[]);
      });
    });

    db.close();

    return queryProm;
  }

  public async getQuiz(quizID: number): Promise<QuizBase> {
    let meta = this.getQuizMeta(quizID);
    let questions = this.getQuestions(quizID);

    return {
      meta: await meta,
      questions: await questions
    } as QuizBase;
  }

  public async getTopFive(quizID: number): Promise<UserScore[]> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let topFiveProm = new Promise<UserScore[]>((resolve, reject) => {
      db.all(`SELECT username, scores.time_sum + penalties.penalty AS score
              FROM player, (
                SELECT player_id, SUM(answer_time) as time_sum
                FROM response
                WHERE question_id IN (
                  SELECT id FROM question
                  WHERE quiz_id = ?
                )
                GROUP BY player_id
              ) AS scores, (
                SELECT failed_qqs.player_id AS player_id, SUM(question.wrong_ans_penalty) AS penalty
                FROM question, (
                  SELECT player_id, question_id
                  FROM response
                  WHERE correct = 0
                  AND question_id IN (
                    SELECT id FROM question
                    WHERE quiz_id = ?
                  )
                ) AS failed_qqs
                WHERE question.id = failed_qqs.question_id
                GROUP BY failed_qqs.player_id
              ) AS penalties
              WHERE player.id = scores.player_id
              AND player.id = penalties.player_id
              ORDER BY score ASC LIMIT 5;`, [quizID, quizID], (err, top) => {
        if (err)
          return reject(err);

        resolve(top);
      });
    });

    db.close();

    return topFiveProm;
  }

  public async getTopFives(quizzes: QuizMeta[]): Promise<UserScore[][]> {
    let topFives: UserScore[][] = [];

    for (const quiz of quizzes) {
      topFives.push(await this.getTopFive(quiz.id));
    }

    return topFives;
  }

  public async getMeanTimes(quizID: number): Promise<number[]> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let queryProm = new Promise<number[]>((resolve, reject) => {
        db.all(`SELECT AVG(answer_time) AS average
                FROM response
                WHERE question_id IN (
                  SELECT id FROM question
                  WHERE quiz_id = ?
                  ORDER BY question_num ASC
                )
                GROUP BY question_id;`, quizID, (err, meanTimes) => {
          if (err)
            return reject(err);

          return resolve(meanTimes.map(meanTime => meanTime.average));
        });
    });

    db.close();

    return queryProm;
  }

  public async getResponses(player: Player, quizID: number): Promise<Response[]> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let responsesProm = new Promise<Response[]>((resolve, reject) => {
      db.all(`SELECT answer, answer_time, correct
              FROM response, (
                SELECT id FROM question
                WHERE quiz_id = ?
                ORDER BY question_num ASC
              ) AS quiz_questions
              WHERE player_id = ?
              AND question_id = quiz_questions.id;`, [quizID, player.id], (err, responses) => {
        if (err)
          return reject(`${err.message}, Błąd w trakcie wyszukiwania statystyk`);

        resolve(responses as Response[]);
      });
    });

    db.close();

    return responsesProm;
  }

  public async getStats(player: Player, quizID: number) {
    let responses = await this.getResponses(player, quizID);
    let meanTimes = await this.getMeanTimes(quizID);
    let questions = await this.getQuestions(quizID);
    let topFive = await this.getTopFive(quizID);

    let timeSum = responses.reduce((prev, cur): number => {
      return prev + cur.answer_time;
    }, 0);

    var penaltySum = responses.reduce((prev, cur, i): number => {
      let curPenalty = cur.correct ? 0 : questions[i].wrong_ans_penalty;
      return prev + curPenalty;
    }, 0);

    return {
      responses: responses,
      time_sum: timeSum,
      penalty_sum: penaltySum,
      mean_times: meanTimes,
      questions: questions,
      top_five: topFive
    }
  }

  public async changePassword(player: Player, password: string) {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let updateProm = new Promise<Response[]>((resolve, reject) => {
      db.run(`UPDATE player
              SET password = ?
              WHERE id = ?`, [password, player.id], (err) => {
        if (err)
          return reject(`${err.message}, Nie udało się zmienić hasła`);

        resolve();
      });
    });

    await updateProm;

    db.close();
  }

  public async consumeResponses(quizID: number, responses: ResponseRaw[], player: Player, time: number) {
    let responsesForInsertion: (number | string)[] = [];

    let questions: QuestionBase[] = await this.getQuestions(quizID);

    for (const [i, response] of responses.entries()) {
      responsesForInsertion = responsesForInsertion.concat([
        response.question_id, // Kolumna question_id
        player.id, // Kolumna player_id
        response.answer, // Kolumna answer
        Math.round(response.time_fraction * time), // Kolumna answer_time
        response.answer === questions[i].correct_answer ? 1 : 0 // Kolumna correct
      ]);
    }

    let placeholders = responses.map((response) => '(?, ?, ?, ?, ?)').join(',');
    let sql = 'INSERT OR IGNORE INTO response (question_id, player_id, answer, answer_time, correct) VALUES ' + placeholders;

    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let insertionProm = new Promise((resolve, reject) => {
      db.run(sql, responsesForInsertion, (err) => {
        if (err)
          return reject(err);

        resolve();
      });
    });

    db.close();

    await insertionProm;
  }

  public async logoutPlayerSessions(player: Player) {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.name);

    let updateProm = new Promise((resolve, reject) => {
      db.all("SELECT * FROM sessions;", (err, sessions) => {
        if (err)
          return reject(err);

        let userSessions = [];

        for (const sessionRow of sessions) {
          if (sessionRow.sess) {
            let session = JSON.parse(sessionRow.sess);
            if (session.passport && session.passport.user === player.id)
              userSessions.push(sessionRow);
          }
        }

        for (const [i, session] of userSessions.entries()) {
          db.run('DELETE FROM sessions WHERE sid = ?;', session.sid, (err) => {
            if (err)
              return reject(err);

            if (i === userSessions.length - 1) {
              return resolve();
            }
          });
        }
      });
    });

    db.close()

    return updateProm;
  }
}

export const quizDB = new QuizDB('quiz_db.db');
