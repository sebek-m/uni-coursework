const stateEnum = Object.freeze({
  WELCOME: 0,
  SUBJECTINFO: 1,
  STAGEONE_INTRO: 2,
  STAGEONE: 3,
  STAGETWO_INTRO: 4,
  STAGETWO: 5,
  STAGETHREE_INTRO: 6,
  STAGETHREE: 7,
  STAGEFOUR_INTRO: 8,
  TASKNAME: 9,
  DESCRIPTIONPART: 10,
  TASKPART: 11,
  THANKS: 12
});

const domEl = Object.freeze({
  nextButton: document.getElementById('next-button'),
  textView: document.getElementById('text-view'),
  infoInputView: document.getElementById('info-input-view'),
  infoForm: document.getElementById('info-form'),
  nicknameInput: document.getElementById('nickname-input'),
  maleRadio: document.getElementById('male'),
  femaleRadio: document.getElementById('female'),
  ageInput: document.getElementById('age-input'),
  linedWindowView: document.getElementById('lined-window-view'),
  top: document.getElementById('top'),
  bottom: document.getElementById('bottom'),
  topExperimental: document.getElementById('top-experimental'),
  bottomControl: document.getElementById('bottom-control'),
  bigText: document.getElementById('big-text'),
  smallText: document.getElementById('small-text'),
  introStory: document.getElementById('intro-story'),
  rules: Array.from(document.getElementsByClassName('rule')),
  extraDescriptions: Array.from(document.getElementsByClassName('extra-description')),
  cards: Array.from(document.getElementsByClassName('card')),
  cardTask: document.getElementById('card-task'),
  body: document.body,
  mainWindow: document.getElementById('main-window'),
  bottomText: document.getElementById('bottom-text')
});

function hideElement(el) {
  el.style.display = 'none';
}

function showElement(el, display = 'initial') {
  el.style.display = display;
  el.style.visibility = 'initial';
}

function switchView(prev, next, display = 'initial') {
  hideElement(prev);
  showElement(next, display);
}

function hideNextButton() {
  hideElement(domEl.nextButton);
}

function showNextButton() {
  showElement(domEl.nextButton);
}

function disableNextButton() {
  domEl.nextButton.disabled = true;
}

function enableNextButton() {
  domEl.nextButton.disabled = false;
}

function setTextViewText(big, small) {
  domEl.bigText.textContent = big;
  domEl.smallText.textContent = small;
}

function shuffleArray(arr) {
  for (var i = arr.length - 1; i >= 0; i--) {
    let j = Math.floor(Math.random() * (i + 1));
    let temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

function extractSelectedTasks(tasks) {
  return tasks.filter((rawTask) => { return rawTask.selected; })
              .map((task) => { return task.number; });
}

function setUpTaskOrder(first, rest, second = false) {
  let arr = [];

  arr.push(first);
  if (second !== false) {
    arr.push(second);
  }

  shuffleArray(rest);
  arr = arr.concat(rest);

  return arr;
}

function fillCards(task) {
  let cardOrder = ['p', 'non_p', 'q', 'non_q'];
  shuffleArray(cardOrder);

  for (let i = 0; i < domEl.cards.length; i++) {
    domEl.cards[i].textContent = task[cardOrder[i]];
  }

  return cardOrder;
}

function fillTask(task, mode) {
  domEl.introStory.textContent = task.intro_story;
  domEl.rules[0].textContent = domEl.rules[1].textContent = `${task.rule}`;
  domEl.extraDescriptions[0].textContent = task.extra_description + " " +
                               task.card_selection_task + 
                               " Reguła ta prezentowana jest jeszcze raz poniżej.";
  domEl.extraDescriptions[1].textContent = task.extra_description;
  domEl.cardTask.textContent = task.card_selection_task;

  if (mode === 1) {
    domEl.topExperimental.style.display = 'none';
    domEl.bottomControl.style.display = 'initial';
  }
  else {
    domEl.topExperimental.style.display = 'initial';
    domEl.bottomControl.style.display = 'none';
  }

  return fillCards(task);
}

function resetCards(cardSelected) {
  for (let i = 0; i < cardSelected.length; i++) {
    cardSelected[i] = false;
    domEl.cards[i].dataset.selected = false;
  }
}

function setBGRed() {
  domEl.body.style.backgroundColor = '#e74c3c';
  domEl.mainWindow.style.backgroundColor = '#ffcccb';
}

function setBGWhite() {
  domEl.body.style.backgroundColor = '#FFFFFF';
  domEl.mainWindow.style.backgroundColor = '#f3f3f3';
}

function checkIfRhythmic(GILClicks) {
  if (GILClicks.length < 4) {
    return false;
  }

  let X = 0;
  let GILDifferences = [];
  for (let i = GILClicks.length - 4; i < GILClicks.length - 1; i++) {
    let diff = GILClicks[i + 1] - GILClicks[i];
    X += diff;
    GILDifferences.push(diff);
  }

  X = X / 3;

  let Q = 0;
  for (let i = 0; i < GILDifferences.length; i++) {
    let diff = GILDifferences[i] - X;
    Q += diff * diff;
  }

  return Q < 10000;
}

function switchBGColor(state, mode, GILClicks) {
  if ((mode === 3 || state !== stateEnum.TASKPART) && checkIfRhythmic(GILClicks)) {
    domEl.body.style.backgroundColor = '#2ecc71';
    domEl.mainWindow.style.backgroundColor = '#d5f5e3';
  }
  else {
    setBGWhite();
  }
}

export { stateEnum, domEl, switchView, hideNextButton,
         showNextButton, setTextViewText, hideElement,
         showElement, extractSelectedTasks, setUpTaskOrder,
         fillTask, resetCards, setBGRed, setBGWhite, switchBGColor,
         disableNextButton, enableNextButton };