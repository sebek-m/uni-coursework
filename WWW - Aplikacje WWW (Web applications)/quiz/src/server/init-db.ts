import { quizDB } from './db-interaction'
import { AnonymousQuizBase } from '../interfaces'
import * as fs from 'fs';
import * as path from 'path';

(async () => {
  try {
    await quizDB.initDB();

    await quizDB.addPlayer('user1', 'user1');
    await quizDB.addPlayer('user2', 'user2');

    const quizzesPath = path.join(__dirname, '../../quizzes');
    const quizzes = fs.readdirSync(quizzesPath);

    for(const quizFileName of quizzes) {
      let realQuizPath = path.join(quizzesPath, quizFileName);
      let quizFileContents = fs.readFileSync(realQuizPath).toString();
      let quizObj: AnonymousQuizBase = JSON.parse(quizFileContents);

      await quizDB.addQuiz(quizObj);
    }
  } catch (e) {
    console.log(e);
  }
})();