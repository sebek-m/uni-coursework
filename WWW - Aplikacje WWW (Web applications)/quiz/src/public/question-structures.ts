/*
  Struktury reprezentujące pytanie
*/
import Stopwatch from "./stopwatch"
import { QuestionBase } from "../interfaces"

/*
  Pytanie do rozwiązywania
*/
export class SolvedQuestion {
  readonly base: QuestionBase;

  private answer: string;
  private valid: boolean;

  private timer: Stopwatch;

  /* 
    Div w widoku quizu, zawierający pytanie,
    informację o karze i pole na odpowiedź.
    Inicjalizujemy dopiero w metodzie prepare()
    klasy SolvedQuiz
  */
  private DOM!: HTMLDivElement;

  constructor(base: QuestionBase) {
    this.base = base;
    this.timer = new Stopwatch();
    this.answer = "";
    this.valid = false;
  }

  public setAnswer(value: string) {
    this.answer = value;
  }

  public getAnswer(): string {
    return this.answer;
  }

  public setDOM(el: HTMLDivElement) {
    this.DOM = el;
  }

  /*
    Uruchamia stoper pytania i pokazuje je.
  */
  public show() {
    this.timer.start();
    this.DOM.style.display = "initial";
  }

  /*
    Pauzuje stoper pytania i chowa je.
  */
  public hide() {
    this.timer.stop();
    this.DOM.style.display = "none";
  }

  /*
    Zatrzymuje stoper pytania.
  */
  public stopTimer() {
    this.timer.stop();
  }

  /*
    Sprawdza, czy wartość wpisana wcześniej do pola odpowiedzi
    jest liczbą całkowitą.
  */
  public checkIfValid(): boolean {
    let validInputRegex = /(^([-]?[1-9]\d*|0)$)/;
    if (!validInputRegex.test(this.answer)) {
      this.valid = false;
    }
    else {
      this.valid = true;
    }

    return this.valid;
  }

  public isValid(): boolean {
    return this.valid;
  }

  public getTime(): number {
    return this.timer.getTime();
  }
}