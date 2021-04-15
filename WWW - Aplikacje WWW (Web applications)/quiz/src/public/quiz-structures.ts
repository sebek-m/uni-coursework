/*
  Struktury reprezentujące quiz
*/
import { SolvedQuestion } from "./question-structures"
import Stopwatch from "./stopwatch"
import DomEl from "./dom-elements"
import { msToSecString } from "../utils"
import { QuizMeta, QuizBase, ResponseRaw } from "../interfaces"

/*
  Quiz do rozwiązywania
*/
export class SolvedQuiz {
  private readonly meta: QuizMeta;
  private readonly questions: SolvedQuestion[];

  private timer: Stopwatch;
  private currentQuestion: number;

  // Sumuję czas z poszczególnych pytań, by uniknąć sytuacji,
  // w której czas mierzony przez stoper byłby o kilka ms większy
  // niż suma czasów spędzonych nad poszczególnymi pytaniami.
  private timeSum: number;

  constructor(base: QuizBase) {
    this.questions = new Array<SolvedQuestion>(base.questions.length);
    for (let i = 0; i < this.questions.length; i++) {
      this.questions[i] = new SolvedQuestion(base.questions[i]);
    }

    this.meta = base.meta;

    this.timer = new Stopwatch(DomEl.stopwatch);
    this.currentQuestion = 0;

    this.timeSum = 0;
  }

  /*
    Sprawdza, czy na wszystkie pytania podano już dopuszczalną odpowiedź.
  */
  private checkIfAllValid(): boolean {
    let valid = true;

    for (let i = 0; valid && i < this.questions.length; i++) {
      if (!this.questions[i].isValid()) {
        valid = false;
      }
    }

    return valid;
  }

  /*
    Zwraca napis reprezentujący numer aktualnego pytania (np. "2/8").
  */
  private formatQuestionNum(): string {
    return (this.currentQuestion + 1) + "/" + this.questions.length;
  }

  /*
    Pokazuje następne pytanie i ewentualnie dezaktywuje odpowiednią strzałkę.
  */
  public nextQuestion() {
    this.questions[this.currentQuestion].hide();
    this.currentQuestion++;
    this.questions[this.currentQuestion].show();

    DomEl.questionNumber.textContent = this.formatQuestionNum();

    if (this.currentQuestion === this.questions.length - 1) {
      DomEl.rightArrow.disabled = true;
    }

    if (this.currentQuestion === 1) {
      DomEl.leftArrow.disabled = false;
    }
  }

  /*
    Pokazuje poprzednie pytanie i ewentualnie dezaktywuje odpowiednią strzałkę.
  */
  public previousQuestion() {
    this.questions[this.currentQuestion].hide();
    this.currentQuestion--;
    this.questions[this.currentQuestion].show();

    DomEl.questionNumber.textContent = this.formatQuestionNum();

    if (this.currentQuestion === 0) {
      DomEl.leftArrow.disabled = true;
    }

    if (this.currentQuestion === this.questions.length - 2) {
      DomEl.rightArrow.disabled = false;
    }
  }

  /*
    Sprawdza to, co wpisano w pole odpowiedzi
    i, jeśli już wszystkie pola są wypełnione,
    to umożliwia kliknięcie przycisku STOP.
  */
  private inputHandler(ev: Event) {
    let inputEl = ev.target as HTMLInputElement;
    let pos = Number(inputEl.name);
    let curValue = this.questions[pos].getAnswer();
    
    let inputRegex = /(^$|^([-]?[1-9]\d*|0|-)$)/;
    if (!inputRegex.test(inputEl.value)) {
      inputEl.value = curValue;
    }
    else {
      this.questions[pos].setAnswer(inputEl.value);

      let isValid = this.questions[pos].checkIfValid();
      if (isValid) {
        if (this.checkIfAllValid()) {
          DomEl.stopButton.disabled = false;
        }
      }
      else {
        DomEl.stopButton.disabled = true;
      }
    }
  }

  /*
    Przygotowuje quiz do rozwiązania
  */
  public prepare() {
    for (let i = 0; i < this.questions.length; i++) {
      let question = this.questions[i].base.content;
      /*
        Podmieniamy symbole matematyczne
      */
      question = question.replace(/\*/g, "&times;");
      question = question.replace(/\:/g, "&divide;");

      /*
        Tworzymy div z pytaniem i polem na odpowiedź
      */
      let questionAndInputEl = document.createElement("div");
      questionAndInputEl.style.display = "none";

      let penaltyEl = document.createElement("p");
      penaltyEl.innerHTML = "Kara za błędną odpowiedź: " +
                            msToSecString(this.questions[i].base.wrong_ans_penalty);

      questionAndInputEl.appendChild(penaltyEl);

      let questionEl = document.createElement("p");
      questionEl.innerHTML = question;

      questionAndInputEl.appendChild(questionEl);

      let inputField = document.createElement("input");
      inputField.type = "text";
      inputField.name = i.toString();

      inputField.oninput = (ev) => { this.inputHandler(ev); };

      questionAndInputEl.appendChild(inputField);

      this.questions[i].setDOM(questionAndInputEl);
      DomEl.questionInputSection.appendChild(questionAndInputEl);
    }

    DomEl.questionNumber.textContent = this.formatQuestionNum();

    DomEl.quizTitleInGame.innerHTML = this.meta.title;
    DomEl.quizIntroInGame.innerHTML = this.meta.description;
  }

  /*
    Uruchamia quiz, czyli uruchamia odpowiednie stopery
    i przechodzi do widoku quizu.
  */
  public start() {
    this.questions[this.currentQuestion].show();

    DomEl.mainPreQuizScreen.style.display = "none";
    DomEl.mainQuizScreen.style.display = "grid";

    DomEl.highScoresSection.style.display = "none";
    DomEl.quizProgressSection.style.display = "flex";

    this.timer.start();
  }

  /*
    Tworzy tablicę obiektów reprezentującą odpowiedzi na pytania
    wraz z ułamkiem całkowitego czasu, w którym udzielono odpowiedzi
    na pytanie.
  */
  private readResults(): ResponseRaw[] {
    let responses: ResponseRaw[] = [];

    for (const q of this.questions) {
      this.timeSum += q.getTime();
    }

    for (const q of this.questions) {
      if (!q.isValid())
        throw Error('Ktoś wcisnął stop bez podawania dupuszczalnej odpowiedzi, prawdopodobnie hackując');

      responses.push({
        question_id: q.base.id,
        answer: q.getAnswer(),
        time_fraction: q.getTime() / this.timeSum
      } as ResponseRaw);
    }

    return responses;
  }

  /*
    Zatrzymuje quiz, wypełnia widok z wynikiem quizu i go wyświetla.
  */
  public async stop() {
    this.questions[this.currentQuestion].stopTimer();
    this.timer.stop();

    let responses = this.readResults();

    let response = await fetch('/submit-responses', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json;charset=utf-8'
      },
      body: JSON.stringify({ quizID: this.meta.id, responses: responses })
    });
    if (response.ok)
      window.location.replace("/results");
    else
      window.location.replace("/");
  }
}