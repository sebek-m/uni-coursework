/*
  Klasa reprezentująca stoper.
*/

export default class Stopwatch {
  private time: number;
  private interval?: number;
  private startTime?: number;
  private pauseTime?: number;
  private pauseSum: number;
  private DOM?: HTMLElement;

  /*
    Możemy przypisać stoperowi jakiś element HTML,
    jeśli chcemy go gdzieś wyświetlać.
  */
  constructor(DOM?: HTMLElement) {
    this.time = 0;
    this.pauseSum = 0;
    this.DOM = DOM;
  }

  /*
    Tworzy napis reprezentujący aktualny czas stopera,
    dbając o stałą liczbę cyfr.
  */
  private formatTime(): string {
    let dateTime = new Date(this.time);
    let min = String(dateTime.getMinutes());
    let sec = String(dateTime.getSeconds());
    let ms = String(dateTime.getMilliseconds());

    if (min.length < 2) {
      min = "0" + min;
    }

    if (sec.length < 2) {
      sec = "0" + sec;
    }

    while (ms.length < 3) {
      ms = "0" + ms;
    }

    return min + ":" + sec + "." + ms;
  }

  /*
    Funkcja wywoływana przez interval. Aktualizuje
    czas stopera.
  */
  private updateTime() {
    /*
      Sprawdzamy, czy interval jest ustawiony, 
      czyli czy stoper jest włączony.
    */
    if (this.interval) {
      let now = Date.now();

      // Wiemy, że startTime nie jest niezdefiniowana,
      // bo inicjalizujemy ją przed ustawieniem interval.
      this.time = now - this.startTime! - this.pauseSum;
    }

    if (this.DOM) {
      this.DOM.textContent = this.formatTime();
    }
  }

  /*
    Uruchamia lub odpauzowuje stoper.
  */
  public start() {
    if (!this.interval) {
      let now = Date.now();

      /*
        Sprawdzamy, czy to pierwsze uruchomienie stopera.
      */
      if (!this.startTime) {
        this.startTime = now;
      }

      /*
        Sprawdzamy, czy stoper był kiedyś zapauzowany.
      */
      if (this.pauseTime) {
        this.pauseSum += now - this.pauseTime;
      }

      this.interval = window.setInterval(this.updateTime.bind(this), 10);
    }
  }

  /*
    Zatrzymuje/pauzuje stoper.
  */
  public stop() {
    if (this.interval) {
      clearInterval(this.interval);
      this.interval = 0;
      this.pauseTime = Date.now();
    }
  }

  public getTime(): number {
    return this.time;
  }
}