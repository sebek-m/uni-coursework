/*
  Informacje ogólne o quizie, który nie znajduje się
  w bazie danych, więc nie ma jeszcze przypisanego id.
*/
export interface AnonymousQuizMeta {
  readonly title: string;
  readonly description: string;
}

/*
  Informacje ogólne o quizie, który już znajduje się
  w bazie danych.
*/
export interface QuizMeta extends AnonymousQuizMeta {
  readonly id: number
}

/*
  Podstawowa zawartość pytania, które jeszcze nie
  znajduje się w bazie danych (nie ma id)
*/
export interface AnonymousQuestionBase {
  readonly content: string;
  readonly correct_answer: string;
  readonly wrong_ans_penalty: number;
}

/*
  Pytanie z bazy danych, ma już ID
*/
export interface QuestionBase extends AnonymousQuestionBase {
  readonly id: number
}

/*
  Odzwierciedla strukturę JSONa zawierającego
  quiz przed wpisaniem go do bazy danych.
*/
export interface AnonymousQuizBase {
  readonly meta: AnonymousQuizMeta;
  readonly questions: AnonymousQuestionBase[];
}

/*
  Quiz, który znajduje się już w bazie
*/
export interface QuizBase {
  readonly meta: QuizMeta;
  readonly questions: QuestionBase[];
}

/*
  Struktura będąca częścią JSONa,
  w którym klient przysyła niesprawdzone odpowiedzi
*/
export interface ResponseRaw {
  readonly question_id: number;
  readonly answer: string;
  readonly time_fraction: number;
}

/*
  Interfejs reprezentujący statystyki pojedynczego pytania
*/
export interface Response {
  readonly answer: string;
  readonly answer_time: number;
  readonly correct: boolean;
}

/*
  Interfejs do budowania tabeli najlepszych wyników
*/
export interface UserScore {
  readonly username: string;
  readonly score: number;
}

/*
  Odzwierciedla JSON przesyłany do klienta,
  by ten mógł wypełnić stronę główną,
  która zawiera listę dostępnych quizów
  i najlepsze wyniki innych graczy.
*/
export interface QuizzesAndScores {
  readonly quizzes: QuizMeta[];
  readonly top_fives: UserScore[][];
}