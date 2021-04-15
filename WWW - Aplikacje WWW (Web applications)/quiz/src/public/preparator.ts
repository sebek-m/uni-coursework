/*
  Funkcje wykonywane przed wybraniem quizu.
*/

import { QuizBase, QuizMeta, UserScore, QuizzesAndScores } from "../interfaces"
import { msToSecString } from "../utils"
import DomEl from "./dom-elements"

/*
  Tworzy listę najlepszych wyników dla pojedynczego quizu o podanym id.
*/
function createTopFiveTable(topFive: UserScore[]): HTMLElement {
  if (topFive.length === 0) {
    let noScores = document.createElement("div");
    noScores.textContent = "BRAK WYNIKÓW";
    noScores.style.display = "none";
    return noScores;
  }
  
  let table = document.createElement("table");
  table.classList.add('top-five');

  table.style.display = "none";

  let headerRow = document.createElement("tr")

  let placeHeader = document.createElement("th");
  let usernameHeader = document.createElement("th");
  let scoreHeader = document.createElement("th");

  placeHeader.textContent = "Miejsce";
  usernameHeader.textContent = "Nazwa";
  scoreHeader.textContent = "Wynik";

  headerRow.appendChild(placeHeader);
  headerRow.appendChild(usernameHeader);
  headerRow.appendChild(scoreHeader);

  for (const [i, score] of topFive.entries()) {
    let curRow = document.createElement("tr");

    let placeCell = document.createElement("td");
    let usernameCell = document.createElement("td");
    let scoreCell = document.createElement("td");

    placeCell.textContent = String(i + 1);
    usernameCell.textContent = score.username;
    scoreCell.textContent = msToSecString(score.score);

    curRow.appendChild(placeCell);
    curRow.appendChild(usernameCell);
    curRow.appendChild(scoreCell);

    table.appendChild(curRow);
  }

  return table;
}

/*
  Dodaje do strony (na początku ukryte) listy najlepszych wyników poszczególnych quizów.
*/
function fillAllHighScores(topFives: UserScore[][]) {
  for (const scores of topFives) {
    let curQuizHighScores = createTopFiveTable(scores);
    DomEl.scoreListsSection.appendChild(curQuizHighScores);
  }
}

/*
  Importuje listę quizów i pasujących do nich najlepszych wyników w formacie JSON z api serwera.
*/
async function importQuizList(): Promise<QuizzesAndScores> {
  let response = await fetch('/api/unsolved-quizzes');
  if (!response.ok)
    throw Error('Nie udało się pobrać quizów');

  return await response.json() as QuizzesAndScores;
}

function fillQuizList(quizzes: QuizMeta[]) {
  for (const quiz of quizzes) {
    let curQuizOption = document.createElement("option");

    curQuizOption.dataset.id = String(quiz.id);
    curQuizOption.dataset.description = quiz.description;
    curQuizOption.textContent = quiz.title;
  
    DomEl.quizSelect.appendChild(curQuizOption);
  }
}

/*
  Pokazuje opis danego quizu oraz najlepsze wyniki
*/
export function updateQuizInfo(prevIndex: number) {
  let prevQuizScores = DomEl.scoreListsSection.children[prevIndex] as HTMLOListElement;
  prevQuizScores.style.display = "none";

  let curIndex = DomEl.quizSelect.selectedIndex;

  let chosenOption = DomEl.quizSelect.options[curIndex]; 

  DomEl.quizIntro.textContent = chosenOption.dataset.description ? chosenOption.dataset.description : '';
  DomEl.scoresQuizName.textContent = chosenOption.textContent;

  let quizScores = DomEl.scoreListsSection.children[curIndex] as HTMLElement;
  quizScores.style.display = "initial";

  return curIndex;
}

export function reloadQuizStyle(quizLength: number) {
  DomEl.leftArrow.disabled = true;
  DomEl.rightArrow.disabled = quizLength > 1 ? false : true;
  DomEl.stopButton.disabled = true;
}

export async function importQuiz(quizID: number): Promise<any> {
  let response = await fetch(`/api/quizzes/${quizID}`);
  if (!response.ok)
    throw Error('Nie udało się pobrać quizu');

  return await response.json();
}

export function showError(error: string) {
  let errMessage = document.createElement("div");
  errMessage.id = "error-message";
  errMessage.textContent = error;
  
  DomEl.quizIntro.appendChild(errMessage);
}

async function fillMainPage() {
  let quizzesAndScores = await importQuizList();

  await Promise.all([fillQuizList(quizzesAndScores.quizzes),
                     fillAllHighScores(quizzesAndScores.top_fives)]);

  DomEl.loadOverlay.style.display = "none";
}

export let mainPageFilled = fillMainPage();