/*
  Stałe zapewniające dostęp do elementów HTML
*/
export default abstract class DomEl {
  // Elementy widoku przed rozpoczęciem quizu.
  public static readonly mainPreQuizScreen = document.getElementsByClassName("main-pre-quiz-screen")[0] as HTMLDivElement;
  public static readonly quizIntro = document.getElementsByClassName("quiz-intro")[0] as HTMLParagraphElement;
  public static readonly quizSelect = document.getElementsByClassName("quiz-select")[0] as HTMLSelectElement;
  public static readonly startButton = document.getElementsByClassName("start-button")[0] as HTMLButtonElement;

  // Elementy widoku w trakcie quizu
  public static readonly mainQuizScreen = document.getElementsByClassName("main-quiz-screen")[0] as HTMLDivElement;
  public static readonly questionNumber = document.getElementsByClassName("question-number")[0] as HTMLDivElement;
  public static readonly leftArrow = document.getElementsByClassName("left-arrow")[0] as HTMLButtonElement;
  public static readonly rightArrow = document.getElementsByClassName("right-arrow")[0] as HTMLButtonElement;
  public static readonly questionInputSection = document.getElementsByClassName("question-and-input-section")[0] as HTMLDivElement;
  public static readonly stopButton = document.getElementsByClassName("stop-button")[0] as HTMLButtonElement;
  public static readonly cancelButton = document.getElementsByClassName("cancel-button")[0] as HTMLButtonElement;

  // Elementy widoku po zakończeniu quizu
  public static readonly mainPostQuizScreen = document.getElementsByClassName("main-post-quiz-screen")[0] as HTMLDivElement;
  public static readonly quizTitlePostGame = document.getElementsByClassName("quiz-title-post-game")[0] as HTMLHeadingElement;
  public static readonly quizIntroPostGame = document.getElementsByClassName("quiz-intro-post-game")[0] as HTMLParagraphElement;
  public static readonly questionResults = document.getElementsByClassName("question-results")[0] as HTMLTableElement;
  public static readonly scoreTimeSum = document.getElementsByClassName("score-time-sum")[0] as HTMLParagraphElement;
  public static readonly scorePenaltySum = document.getElementsByClassName("score-penalty-sum")[0] as HTMLParagraphElement;
  public static readonly scoreSum = document.getElementsByClassName("score-sum")[0] as HTMLParagraphElement;
  public static readonly saveScoreButton = document.getElementsByClassName("save-score-button")[0] as HTMLButtonElement;
  public static readonly saveStatsButton = document.getElementsByClassName("save-stats-button")[0] as HTMLButtonElement;

  // Elementy tabeli wyników
  public static readonly highScoresSection = document.getElementsByClassName("high-scores-section")[0] as HTMLDivElement;
  public static readonly scoresQuizName = document.getElementsByClassName("scores-quiz-name")[0] as HTMLParagraphElement;
  public static readonly scoreListsSection = document.getElementsByClassName("score-lists-section")[0] as HTMLDivElement;

  // Elementy informacji wyświetlanych w trakcie quizu
  public static readonly quizProgressSection = document.getElementsByClassName("quiz-progress-section")[0] as HTMLDivElement;
  public static readonly quizTitleInGame = document.getElementsByClassName("quiz-title-in-game")[0] as HTMLHeadingElement;
  public static readonly quizIntroInGame = document.getElementsByClassName("quiz-intro-in-game")[0] as HTMLParagraphElement;
  public static readonly stopwatch = document.getElementsByClassName("stopwatch")[0] as HTMLDivElement;

  public static readonly loadOverlay = document.getElementById("loadOverlay") as HTMLDivElement;
}