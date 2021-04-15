/*
  Wywołanie głównych funkcji oraz stworzenie handlerów zdarzeń
*/
import { mainPageFilled, updateQuizInfo, importQuiz, reloadQuizStyle, showError } from "./preparator"
import { SolvedQuiz } from "./quiz-structures"
import DomEl from "./dom-elements"

let selectedQuizIndex: number;

mainPageFilled.then(() => {
  selectedQuizIndex = DomEl.quizSelect.selectedIndex;

  updateQuizInfo(selectedQuizIndex);
}).catch((err) => {
  console.log(err.message);
});

/*
  Zmiana wyboru quizu
*/
DomEl.quizSelect.onchange = () => {
  selectedQuizIndex = updateQuizInfo(selectedQuizIndex);
}

let activeQuiz: SolvedQuiz;

/*
  Wciśnięcie przycisku rozpoczynania quizu.
*/
DomEl.startButton.onclick = async () => {
  try {
    let chosenQuizID = DomEl.quizSelect.options[selectedQuizIndex].dataset.id;
    if (!chosenQuizID)
      throw Error('Opcja na liście quizów nie ma pola id');

    let rawChosenQuiz = await importQuiz(parseInt(chosenQuizID, 10));
    if (rawChosenQuiz.error) {
      showError(rawChosenQuiz.error);
      return;
    }

    reloadQuizStyle(rawChosenQuiz.questions.length);

    activeQuiz = new SolvedQuiz(rawChosenQuiz);
    activeQuiz.prepare();
    activeQuiz.start();
  } catch (e) {
    console.log(e);
  }
}

/*
  Przejście do następnego pytania
*/
DomEl.rightArrow.onclick = () => {
  if (DomEl.rightArrow.disabled === false) {
    activeQuiz.nextQuestion();
  }
}

/*
  Przejście do poprzedniego pytania
*/
DomEl.leftArrow.onclick = () => {
  if (DomEl.leftArrow.disabled === false) {
    activeQuiz.previousQuestion();
  }
}

/*
  Wciśnięcie przycisku do anulowania quizu
*/
DomEl.cancelButton.onclick = () => {
  location.reload();
}

/*
  Wciśnięcie przycisku do zatrzymania quizu
*/
DomEl.stopButton.onclick = () => {
  if (DomEl.stopButton.disabled === false) {
    activeQuiz.stop();
  }
}