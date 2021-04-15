const alternativeNumber: number = 1023;
const lampKeys: {[key: string]: number} = {
  'KeyQ': 0,
  'KeyW': 1,
  'KeyE': 2,
  'KeyR': 3,
  'KeyV': 4,
  'KeyN': 5,
  'KeyU': 6,
  'KeyI': 7,
  'KeyO': 8,
  'KeyP': 9
}

const lightArrayEl = document.getElementsByClassName('light') as HTMLCollectionOf<HTMLDivElement>;
const pauseMenuEl = document.getElementById('pause-menu') as HTMLDivElement;
const resumeSessionButtonEl = document.getElementById('resume-session-button') as HTMLButtonElement;
const endSessionButtonEl = document.getElementById('end-session-button') as HTMLButtonElement;
const researcherInterfaceEl = document.getElementById('researcher-interface') as HTMLDivElement;
const researcherInterfaceCloseEl = document.getElementById('interface-close') as HTMLDivElement;
const ageSexInputViewEl = document.getElementById('age-sex-input-view') as HTMLDivElement;
const nicknameInputViewEl = document.getElementById('nickname-input-view') as HTMLDivElement;
const nicknameInputEl = document.getElementById('nickname-input') as HTMLInputElement;
const acceptNicknameEl = document.getElementById('accept-nickname') as HTMLButtonElement;
const ageInputEl = document.getElementById('age-input') as HTMLInputElement;
const sexSelectEl = document.getElementById('sex-select') as HTMLSelectElement;
const acceptAgeSexEl = document.getElementById('accept-age-sex') as HTMLButtonElement;
const preSessionViewEl = document.getElementById('pre-session-view') as HTMLDivElement;
const knownUserWelcomeEl = document.getElementById('known-user-welcome') as HTMLDivElement;
const newUserWelcomeEl = document.getElementById('new-user-welcome') as HTMLDivElement;
const startTrainingButtonEl = document.getElementById('start-training-button') as HTMLButtonElement;
const startSessionButtonEl = document.getElementById('start-session-button') as HTMLButtonElement;
const welcomeTextEl = document.getElementById('welcome-text') as HTMLHeadingElement;
const inProgressViewEl = document.getElementById('in-progress-view') as HTMLDivElement;
const inBetweenViewEl = document.getElementById('in-between-view') as HTMLDivElement;
const settingsFormEl = document.getElementById('settings-form') as HTMLFormElement;
const saveForParticipantButtonEl = document.getElementById('save-for-participant-button') as HTMLButtonElement;
const saveAsDefaultButtonEl = document.getElementById('save-as-default-button') as HTMLButtonElement;
const timeTInputEl = document.getElementById('time-t-input') as HTMLInputElement;
const confNumInputEl = document.getElementById('conf-num-input') as HTMLInputElement;
const feedbackModeRadioEl = document.getElementById('feedback-mode') as HTMLInputElement;
const noFeedbackModeRadioEl = document.getElementById('no-feedback-mode') as HTMLInputElement;
const overallTimeVersionEl = document.getElementById('overall-time-version') as HTMLInputElement;
const finalTimeVersionEl = document.getElementById('final-time-version') as HTMLInputElement;
const endScreenViewEl = document.getElementById('end-screen-view') as HTMLDivElement;
const backToStartEl = document.getElementById('back-to-start') as HTMLButtonElement;
const endMessageEl = document.getElementById('end-message') as HTMLParagraphElement;
const allPatternsInfoEl = document.getElementById('all-patterns-info') as HTMLDivElement;

let loggedIn: boolean = false;
let newUser: boolean;
let inProgress: boolean = false;
let training: boolean = false;
let paused: boolean = false;

let nickname: string;
let participantId: number;
let shownPatterns: number[];
let shownPatternsSet: Set<number>;

let waitAfter: boolean;
let tValue: number;
let trialCount: number;
let feedback: boolean;

let sessionId: string;
let remainingPatterns: Pattern[];
let remainingPatternIdsSet: Set<number>;
let currentPattern: Pattern;
let currentTimeout: number;

let keysPressed: Map<string, boolean> = new Map();

interface Answer {
  time: number;
  correct: boolean;
}

class Pattern {
  sourceNumber: number;

  pattern: boolean[];
  lit: number;

  answers: (Answer | null)[];

  start?: number;

  constructor(sourceNum: number) {
    this.sourceNumber = sourceNum;

    this.answers = new Array(10);
    this.answers.fill(null);

    this.pattern = new Array(10);
    this.lit = 0;

    for (let i = 9; i >= 0; i--) {
      if (sourceNum % 2) {
        this.pattern[i] = true;
        this.lit++;
      }
      else {
        this.pattern[i] = false;
      }

      sourceNum = Math.floor(sourceNum / 2);
    }
  }

  private endTrainingStartExperimental() {
    training = false;

    inProgressViewEl.style.display = 'none';
    inBetweenViewEl.style.display = 'initial';

    setTimeout(() => {
      inBetweenViewEl.style.display = 'none';
      beginExperimentalSession();
    }, 5000);
  }

  private showNextOrEnd() {
    if (remainingPatterns.length > 0) {
      updateCurrentPattern();
      currentPattern.show();
    }
    else {
      if (training) {
        this.endTrainingStartExperimental();
      }
      else {
        inProgress = false;
        inProgressViewEl.style.display = 'none';
        endScreenViewEl.style.display = 'flex';
      }
    }
  }

  private setTimer() {
    currentTimeout = window.setTimeout(() => {
      if (inProgress) {
        if (!training) {
          this.submit();
        }
        this.showNextOrEnd();
      }
    }, tValue);
  }

  public show() {
    for (let i = 0; i < lightArrayEl.length; i++) {
      let bulbs = lightArrayEl[i].children as HTMLCollectionOf<HTMLImageElement>;

      bulbs[0].style.display = this.pattern[i] ? 'none' : 'initial';
      bulbs[1].style.display = this.pattern[i] ? 'initial' : 'none';
    }

    this.start = Date.now();

    if (!waitAfter) {
      this.setTimer();
    }
  }

  private saveAnswer(which: number, pressTime: number, correct: boolean) {
    if (this.answers[which] === null) {
      this.answers[which] = {
        time: pressTime,
        correct: correct
      }
    }
  }

  private convertToJSON(): string {
    return JSON.stringify({
      session_id: sessionId,
      pattern: this.sourceNumber,
      start: this.start,
      end: Date.now(),
      answers: this.answers
    });
  }

  private async submit() {
    shownPatternsSet.add(this.sourceNumber);
    console.log(shownPatternsSet);

    let response = await fetch('/submit-pattern-answers', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json;charset=utf-8'
      },
      body: this.convertToJSON()
    });

    if (!response.ok)
      window.location.replace("/");
  }

  private turnLampOff(which: number) {
    let bulbs = lightArrayEl[which].children as HTMLCollectionOf<HTMLImageElement>;

    bulbs[0].style.display = 'initial';
    bulbs[1].style.display = 'none';

    this.pattern[which] = false;
    this.lit--;
  }

  private maybeFinishPresentation() {
    if (this.lit === 0 && waitAfter) {
      this.setTimer();
    }
  }

  public investigateAnswer(keyCode: string, pressTime: number) {
    let which = lampKeys[keyCode];

    if (this.pattern[which]) {
      this.turnLampOff(which);
      this.saveAnswer(which, pressTime, true);
      this.maybeFinishPresentation();
    }
    else if (feedback) {
      new Audio('static/error_sound.wav').play();
      this.saveAnswer(which, pressTime, false);
    }
  }

  public getSourceNumber() {
    return this.sourceNumber;
  }
}

function flushLights() {
  for (let light of lightArrayEl) {
    let bulbs = light.children as HTMLCollectionOf<HTMLImageElement>;

    bulbs[0].style.display = 'initial'
    bulbs[1].style.display = 'none';
  }
}

function updateCurrentPattern() {
  currentPattern = remainingPatterns.pop()!;
  remainingPatternIdsSet.delete(currentPattern.getSourceNumber());
  console.log(remainingPatterns);
  console.log(remainingPatternIdsSet);
}

async function initializeSession(start: number) {
  let response = await fetch('/begin-session', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=utf-8'
    },
    body: JSON.stringify({
      id: participantId,
      start: start,
      feedback_loop: feedback,
      constant_time_version: !waitAfter,
      t_value: tValue,
      trial_count: trialCount
    })
  });

  if (response.ok) {
    let info = await response.json();
    console.log(info);
    sessionId = info.session_id;
  }
  else {
    window.location.replace("/");
  }
}

function createPatternIdsSet(patternList: Pattern[]): Set<number> {
  return new Set(remainingPatterns.map((pattern) => pattern.getSourceNumber()));
}

async function beginExperimentalSession() {
  remainingPatterns = drawRandomUnshownPatterns(shownPatterns, trialCount);
  remainingPatternIdsSet = createPatternIdsSet(remainingPatterns);
  updateCurrentPattern();

  inProgress = true;

  let sessionStart = Date.now();
  await initializeSession(sessionStart);

  currentPattern.show();
  preSessionViewEl.style.display = 'none';
  inProgressViewEl.style.display = 'flex';
}

function beginTrainingSession() {
  remainingPatterns = drawRandomUnshownPatterns([], 5);
  remainingPatternIdsSet = createPatternIdsSet(remainingPatterns);
  updateCurrentPattern();

  inProgress = true;
  training = true;

  console.log(waitAfter);

  currentPattern.show();
  preSessionViewEl.style.display = 'none';
  inProgressViewEl.style.display = 'flex';
}

function showRegistrationScreen() {
  nicknameInputViewEl.style.display = 'none';
  ageSexInputViewEl.style.display = 'initial';
}

function showLoggedInScreen() {
  knownUserWelcomeEl.style.display = 'initial';
  welcomeTextEl.innerText = `Witaj, ${nickname}!`;

  newUser = false;

  if (shownPatterns.length === alternativeNumber) {
    startSessionButtonEl.style.display = 'none';
    allPatternsInfoEl.style.display = 'initial';
  } else {
    startSessionButtonEl.style.display = 'initial';
    allPatternsInfoEl.style.display = 'none';
  }

  nicknameInputViewEl.style.display = 'none';
  preSessionViewEl.style.display = 'initial';
}

function showWelcomeScreen() {
  newUserWelcomeEl.style.display = 'initial';
  welcomeTextEl.innerText = `Witaj, ${nickname}!`;

  newUser = true;

  ageSexInputViewEl.style.display = 'none';
  preSessionViewEl.style.display = 'initial';
}

async function registerParticipant(age: number, sex: string) {
  let response = await fetch('/register-participant', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=utf-8'
    },
    body: JSON.stringify({
      nickname: nickname,
      age: age,
      sex: sex
    })
  });

  if (response.ok) {
    let info = await response.json();
    console.log(info);
    readParticipantInfo(info);
    loggedIn = true;
    showWelcomeScreen();
  }
  else {
    window.location.replace("/");
  }
}

function readParticipantInfo(info: any) {
  participantId = info.id;
  shownPatterns = info.finished_patterns;
  feedback = info.feedback_loop;
  waitAfter = !info.constant_time_version;
  tValue = info.t_value;
  trialCount = info.trial_count;
}

async function sendNickname() {
  let response = await fetch('/login', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=utf-8'
    },
    body: JSON.stringify({
      nickname: nickname
    })
  });

  if (response.ok) {
    let info = await response.json();

    console.log(info);

    if (info.info !== undefined) {
      showRegistrationScreen()
    }
    else {
      loggedIn = true;
      readParticipantInfo(info);
      showLoggedInScreen();
    }
  }
  else {
    window.location.replace("/");
  }
}

function drawOneRandomUnshownPattern(): Pattern {
  let patternId;

  do {
    patternId = Math.floor(Math.random() * alternativeNumber) + 1;
  } while (shownPatternsSet.has(patternId) || remainingPatternIdsSet.has(patternId));

  return new Pattern(patternId);
}

function drawRandomUnshownPatterns(shownList: number[], num: number): Pattern[] {
  shownPatternsSet = new Set(shownList);
  let drawn: Set<number> = new Set();

  const howMany = Math.min(num, alternativeNumber - shownPatternsSet.size);

  for (let i = 0; i < howMany; i++) {
    let patternId;

    do {
      patternId = Math.floor(Math.random() * alternativeNumber) + 1;
    } while (shownPatternsSet.has(patternId) || drawn.has(patternId));

    drawn.add(patternId);
  }

  let patterns: Pattern[] = [];

  for (let id of drawn.values()) {
    patterns.push(new Pattern(id));
  }

  return patterns;
}

function checkKeyPress(keyCode: string, pressTime: number) {
  if (lampKeys[keyCode] !== undefined) {
    currentPattern.investigateAnswer(keyCode, pressTime);
  }
}

async function sendDefaultSettings(settings: any) {
  let response = await fetch('/save-default-settings', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=utf-8'
    },
    body: JSON.stringify(settings)
  });

  if (response.ok) {
    researcherInterfaceEl.style.display = "none";
    settingsFormEl.reset();
  }
  else {
    window.location.replace("/");
  }
}

async function sendParticipantSettings(settings: any) {
  let response = await fetch('/save-participant-settings', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=utf-8'
    },
    body: JSON.stringify(settings)
  });

  if (response.ok) {
    let info = await response.json();
    console.log(info);

    readParticipantInfo(info);

    if (!newUser) {
      showLoggedInScreen();
    }

    researcherInterfaceEl.style.display = "none";
    settingsFormEl.reset();
  }
  else {
    window.location.replace("/");
  }
}

function openInterface() {
  if (loggedIn) {
    saveForParticipantButtonEl.style.display = 'initial';
  }

  researcherInterfaceEl.style.display = "block";
}

function pauseSession() {
  window.clearTimeout(currentTimeout);
  inProgress = false;
  paused = true;

  flushLights();
  pauseMenuEl.style.display = "block";
}

function resumeSession() {
  currentPattern = drawOneRandomUnshownPattern();

  inProgress = true;
  paused = false;

  currentPattern.show();
  pauseMenuEl.style.display = 'none';
}

function endSession() {
  if (training) {
    endMessageEl.innerText = "Zakończono sesję treningową"
  }
  else {
    endMessageEl.innerText = "Zakończono sesję eksperymentalną"
  }
  inProgressViewEl.style.display = 'none';
  endScreenViewEl.style.display = 'flex';
  pauseMenuEl.style.display = 'none';
}

interface Settings {
  id?: number;
  feedback_loop: (boolean | null);
  constant_time_version: (boolean | null);
  t_value: (number | null);
  trial_count: (number | null);
}

function prepareSettingsObject(): Settings {
  return {
    feedback_loop: ((feedbackModeRadioEl.checked || noFeedbackModeRadioEl.checked) ?
                    feedbackModeRadioEl.checked : null),
    constant_time_version: ((overallTimeVersionEl.checked || finalTimeVersionEl.checked) ?
                            overallTimeVersionEl.checked : null),
    t_value: (timeTInputEl.value === "" ? null : timeTInputEl.valueAsNumber),
    trial_count: (confNumInputEl.value === "" ? null : confNumInputEl.valueAsNumber)
  }
}

document.onkeydown = (e) => {
  let pressTime: number = Date.now();
  keysPressed.set(e.code, true);

  if (inProgress) {
    if (keysPressed.size === 3 &&
      keysPressed.has('KeyZ') &&
      keysPressed.has('KeyX') &&
      keysPressed.has('KeyN')) {
      pauseSession();
    }
    else {
      checkKeyPress(e.code, pressTime);
    }
  }
  else if (!paused &&
           keysPressed.size === 3 &&
           keysPressed.has('KeyZ') &&
           keysPressed.has('KeyX') &&
           keysPressed.has('KeyM')) {
    openInterface();
  }
}

document.onkeyup = (e) => {
  keysPressed.delete(e.code);
}

researcherInterfaceCloseEl.onclick = () => {
  researcherInterfaceEl.style.display = "none";
}

acceptNicknameEl.onclick = (e) => {
  e.preventDefault();

  if (nicknameInputEl.value !== '') {
    nickname = nicknameInputEl.value;
    sendNickname();
  }
}

acceptAgeSexEl.onclick = (e) => {
  e.preventDefault();

  if (ageInputEl.value !== undefined && ageInputEl.value !== null && !Number.isNaN(ageInputEl.valueAsNumber) &&
      sexSelectEl.value !== undefined && sexSelectEl.value !== null) {
    registerParticipant(ageInputEl.valueAsNumber, sexSelectEl.value);
  }
}

startSessionButtonEl.onclick = (e) => {
  beginExperimentalSession();
}

startTrainingButtonEl.onclick = (e) => {
  beginTrainingSession();
}

saveAsDefaultButtonEl.onclick = (e) => {
  e.preventDefault();

  if ((timeTInputEl.value === "" || !Number.isNaN(timeTInputEl.valueAsNumber)) &&
      (confNumInputEl.value === "" || !Number.isNaN(confNumInputEl.valueAsNumber))) {
    let settings = prepareSettingsObject();

    console.log(settings);

    sendDefaultSettings(settings);
  }
}

saveForParticipantButtonEl.onclick = (e) => {
  e.preventDefault();

  if ((timeTInputEl.value === "" || !Number.isNaN(timeTInputEl.valueAsNumber)) &&
      (confNumInputEl.value === "" || !Number.isNaN(confNumInputEl.value))) {
    let settings = prepareSettingsObject();
    settings.id = participantId;

    console.log(settings);

    sendParticipantSettings(settings);
  }
}

backToStartEl.onclick = (e) => {
  window.location.replace("/");
}

resumeSessionButtonEl.onclick = (e) => {
  resumeSession();
}

endSessionButtonEl.onclick = (e) => {
  endSession();
}