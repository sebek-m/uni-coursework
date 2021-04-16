import { stateEnum, domEl, switchView, hideNextButton,
         showNextButton, setTextViewText, hideElement,
         showElement, extractSelectedTasks, setUpTaskOrder,
         fillTask, resetCards, setBGRed, setBGWhite, switchBGColor,
         disableNextButton, enableNextButton } from './utils.js';
import { getSettings, getTask, sendData } from './fetches.js'

var settingsPromise = getSettings();
var settingsReceived = false;
var settings;

var nickname;
var gender;
var age;

var currentState = stateEnum.WELCOME;
var currentGILTimeout = null;

var selectedTasks;
var currentTaskNum = 0;
var currentTask;
var cards;
var cardSelected = [false, false, false, false];
var howManySelected = 0;

var taskExecutionStart;
var taskSolutionStart;

var cardSelectionEvents = [[], [], [], []];
var cardDeselectionEvents = [[], [], [], []];

var GILClicks = [];

var data = {
  stageTwo: {
    start: null,
    GILClicks: []
  },
  taskData: []
};

function stopGIL() {
  window.clearTimeout(currentGILTimeout);
  GILClicks = [];
  setBGWhite();
}

function resetGILTimeout() {
  if (currentGILTimeout !== null) {
    window.clearTimeout(currentGILTimeout);
  }

  currentGILTimeout = window.setTimeout(setBGRed,
                      settings.general.k_time * 1000);
}

function finishTask() {
  let taskEnd = Date.now();

  data.taskData.push({
    externalTaskNum: currentTaskNum + 1,
    internalTaskNum: currentTask.task_number,
    taskName: currentTask.task_name,
    executionStart: taskExecutionStart,
    solutionStart: taskSolutionStart,
    end: taskEnd,
    GILClicks: [...GILClicks],
    cardOrder: [...cards],
    selectedCards: [...cardSelected],
    cardSelectionEvents: [[...(cardSelectionEvents[0])], [...(cardSelectionEvents[1])],
                          [...(cardSelectionEvents[2])], [...(cardSelectionEvents[3])]],
    cardDeselectionEvents: [[...(cardDeselectionEvents[0])], [...(cardDeselectionEvents[1])],
                            [...(cardDeselectionEvents[2])], [...(cardDeselectionEvents[3])]]
  });

  stopGIL();
}

function getTargetIndex(cardOrder, i) {
  switch(cardOrder[i]) {
    case 'p':
      return 0;
    case 'non_p':
      return 1;
    case 'q':
      return 2;
    case 'non_q':
      return 3;
    default:
      return -1;
  }
}

function finalCardName(name) {
  switch(name) {
    case 'p':
      return 'P';
    case 'non_p':
      return 'nP';
    case 'q':
      return 'Q';
    case 'non_q':
      return 'nQ';
    default:
      return 'invalid name';
  }
}

function transformCardOrder(cardOrder) {
  return cardOrder.map((card) => finalCardName(card)).join(';');
}

/*
  Transforms data into a format that is easily transformed into csv
  on server-side.
*/
function finalizeData() {
  let finalData = {
    nickname: nickname,
    gender: gender,
    age: age,
    mode: settings.mode,
    stageTwoGILClicks: data.stageTwo.GILClicks.map((click) => click - data.stageTwo.start).join(';'),
    taskData: []
  }

  for (var taskInfo of data.taskData) {
    let finalTaskData = {
      externalTaskNum: taskInfo.externalTaskNum,
      internalTaskNum: taskInfo.internalTaskNum,
      taskName: taskInfo.taskName,
      executionDuration: taskInfo.end - taskInfo.executionStart,
      solutionDuration: taskInfo.end - taskInfo.solutionStart,
      GILClicks: (settings.mode === 1 ? 'n/a' :
                  taskInfo.GILClicks.map((click) => click - taskInfo.solutionStart).join(';')),
      cardOrder: transformCardOrder(taskInfo.cardOrder),
      correct: true,
      selectionTimes: null,
      deselectionTimes: null,
      finalSelectionOrder: null,
      finalSelectionOrderedTimes: null
    }

    /*
      Transform into relative times
    */
    taskInfo.cardSelectionEvents = taskInfo.cardSelectionEvents.map(
                                    (selectionTimes) => selectionTimes.map(
                                      (selection) => selection - taskInfo.solutionStart
                                    )
                                   );
    taskInfo.cardDeselectionEvents = taskInfo.cardDeselectionEvents.map(
                                      (deselectionTimes) => deselectionTimes.map(
                                        (deselection) => deselection - taskInfo.solutionStart
                                      )
                                     );

    /*
      Check if correct
    */
    for (let i = 0; i < taskInfo.selectedCards.length; i++) {
      if ((!taskInfo.selectedCards[i] &&
           (taskInfo.cardOrder[i] == "p" || taskInfo.cardOrder[i] == "non_q")) ||
          (taskInfo.selectedCards[i] &&
           !(taskInfo.cardOrder[i] == "p" || taskInfo.cardOrder[i] == "non_q"))) {
        finalTaskData.correct = false;
      }
    }

    /*
      Reorder selection times so that they match the final data format
    */
    let finalSelectionTimes = [[-1, 'P'], [-1, 'nP'], [-1, 'Q'], [-1, 'nQ']];
    let reorderedSelectionTimes = new Array(4);
    let reorderedDeselectionTimes = new Array(4);
    for (let i = 0; i < taskInfo.cardSelectionEvents.length; i++) {
      let targetIndex = getTargetIndex(taskInfo.cardOrder, i);
      reorderedSelectionTimes[targetIndex] = taskInfo.cardSelectionEvents[i].join(';');
      reorderedDeselectionTimes[targetIndex] = taskInfo.cardDeselectionEvents[i].join(';');
      if (taskInfo.selectedCards[i]) {
        let lastElIndex = taskInfo.cardSelectionEvents[i].length - 1;
        finalSelectionTimes[targetIndex][0] = taskInfo.cardSelectionEvents[i][lastElIndex];
      }
    }

    /*
      Extract final selection info
    */
    let finalSelectionPairs = finalSelectionTimes.filter((pair) => pair[0] != -1)
                                                 .sort((a, b) => a[0] - b[0]);
    finalTaskData.finalSelectionOrder = finalSelectionPairs.map((pair) => pair[1]).join(';');
    finalTaskData.finalSelectionOrderedTimes = finalSelectionPairs.map((pair) => pair[0]).join(';');

    finalTaskData.selectionTimes = reorderedSelectionTimes;
    finalTaskData.deselectionTimes = reorderedDeselectionTimes;

    finalData.taskData.push(finalTaskData);
  }

  return finalData;
}

function switchStageAfterSeconds(instructions, seconds) {
  window.setTimeout(() => {
    if (currentState === stateEnum.STAGETWO) {
      data.stageTwo.GILClicks = [...GILClicks];
    }
    stopGIL();
    currentState++;
    setTextViewText("", instructions);
    switchView(domEl.linedWindowView, domEl.textView, 'flex');
    enableNextButton();
    showNextButton();
  }, seconds * 1000);
}

domEl.nextButton.onclick = async (e) => {
  if (!settingsReceived) {
    settings = await settingsPromise;
    settingsReceived = true;
  }

  switch(currentState) {
    case stateEnum.WELCOME:
      hideNextButton();
      switchView(domEl.textView, domEl.infoInputView, 'flex');
      break;
    case stateEnum.STAGEONE_INTRO:
      hideNextButton();
      hideElement(domEl.textView);
      switchStageAfterSeconds(settings.stage_two.instructions,
                              settings.stage_one.duration);
      resetGILTimeout();
      break;
    case stateEnum.STAGETWO_INTRO:
      hideNextButton();
      hideElement(domEl.textView);
      switchStageAfterSeconds(settings.stage_three.instructions,
                              settings.stage_two.duration);
      data.stageTwo.start = Date.now();
      resetGILTimeout();
      break;
    case stateEnum.STAGETHREE_INTRO:
      domEl.top.style.visibility = 'hidden';
      fillTask({
        intro_story: "",
        rule: "",
        extra_description: "",
        card_selection_task: "",
        p: "",
        non_p: "",
        q: "",
        non_q: ""
      }, 2);
      domEl.bottomText.style.visibility = 'hidden';
      window.setTimeout(() => {
        domEl.bottomText.style.visibility = 'inherit';
      }, settings.stage_three.duration * 1000);
      hideNextButton();
      switchView(domEl.textView, domEl.linedWindowView, 'flex');

      let instructions;
      if (settings.mode === 1) {
        instructions = settings.stage_four.instructions;
      }
      else if (settings.mode === 2) {
        instructions = settings.stage_four.instructions_two;
      }
      else {
        instructions = settings.stage_four.instructions_three;
      }

      switchStageAfterSeconds(instructions,
                              settings.stage_three.duration);
      resetGILTimeout();
      break;
    case stateEnum.STAGEFOUR_INTRO:
      selectedTasks = setUpTaskOrder(settings.stage_four.first_task,
                                     extractSelectedTasks(settings.stage_four.tasks),
                                     settings.stage_four.second_task);

      setTextViewText(`Zadanie ${currentTaskNum + 1}`, "");
      break;
    case stateEnum.TASKNAME:
      currentTask = await getTask(selectedTasks[currentTaskNum]);
      cards = fillTask(currentTask, settings.mode);
      resetCards(cardSelected);
      showElement(domEl.top);
      domEl.bottom.style.visibility = 'hidden';
      cardSelectionEvents = [[], [], [], []];
      cardDeselectionEvents = [[], [], [], []];
      switchView(domEl.textView, domEl.linedWindowView, 'flex');
      taskExecutionStart = Date.now();
      break;
    case stateEnum.DESCRIPTIONPART:
      showElement(domEl.bottom, 'flex');
      if (settings.mode !== 1) {
        resetGILTimeout();
      }
      taskSolutionStart = Date.now();
      howManySelected = 0;
      disableNextButton();
      break;
    case stateEnum.TASKPART:
      finishTask();
      currentTaskNum++;
      if (currentTaskNum < selectedTasks.length) {
        // Continue to next task.
        setTextViewText(`Zadanie ${currentTaskNum + 1}`, "");
      }
      else {
        // All tasks have been finished so the "Thank you" screen is shown.
        setTextViewText("KONIEC", settings.general.thanks_text);
        sendData(finalizeData());
        hideNextButton();
      }
      
      switchView(domEl.linedWindowView, domEl.textView, 'flex');
      break;
    default:
      break;
  }

  if (currentState != stateEnum.TASKPART) {
    currentState++;
  }
  else if (currentTaskNum < selectedTasks.length) {
    currentState = stateEnum.TASKNAME;
  }
  else {
    currentState = stateEnum.THANKS;
  }
}

for (let i = 0; i < domEl.cards.length; i++) {
  let card = domEl.cards[i];
  card.onclick = (e) => {
    let time = Date.now();

    if (card.dataset.selected == "false") {
      card.dataset.selected = true;
      cardSelected[i] = true;
      cardSelectionEvents[i].push(time);
      howManySelected++;
    }
    else {
      card.dataset.selected = false;
      cardSelected[i] = false;
      cardDeselectionEvents[i].push(time);
      howManySelected--;
    }

    if (howManySelected == 0) {
      disableNextButton();
    }
    else if (howManySelected == 1) {
      enableNextButton();
    }
  };
}

// GIL click collection
window.oncontextmenu = (e) => {
  e.preventDefault();

  let clickMoment = Date.now();

  if ((currentState !== stateEnum.STAGEONE &&
      currentState !== stateEnum.STAGETWO &&
      currentState !== stateEnum.STAGETHREE &&
      currentState !== stateEnum.TASKPART) ||
      (currentState === stateEnum.TASKPART &&
       settings.mode === 1) ||
      domEl.cards.some(card => card === e.target)) {
    return;
  }

  resetGILTimeout();
  GILClicks.push(clickMoment);
  switchBGColor(currentState, settings.mode, GILClicks);
}

domEl.infoForm.onsubmit = (e) => {
  e.preventDefault();

  nickname = domEl.nicknameInput.value;
  gender = (domEl.maleRadio.checked ? domEl.maleRadio.value : domEl.femaleRadio.value);
  age = domEl.ageInput.value;

  currentState++;

  setTextViewText("", settings.stage_one.instructions);
  switchView(domEl.infoInputView, domEl.textView, 'flex');
  showNextButton();
}