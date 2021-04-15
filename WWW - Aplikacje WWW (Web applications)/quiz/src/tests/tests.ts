import { Builder, ThenableWebDriver, By, until } from 'selenium-webdriver';
import { expect } from 'chai';
import 'mocha';

const addrBase = 'http://127.0.0.1:3000/';

async function maybeLogIn(driver: ThenableWebDriver, password: string) {
  if (await driver.getCurrentUrl() !== addrBase) {
    await driver.get(addrBase);
  }

  try {
    await driver.findElement(By.id('username')).sendKeys('user1');
    await driver.findElement(By.id('password')).sendKeys(password);
    await driver.findElement(By.css('input[type=submit]')).click();
  } catch (e) {
    console.log("Already logged in");
  }

  return driver.wait(until.elementsLocated(By.className('logged-in-as')));
}

async function solveQuiz(driver: ThenableWebDriver) {
  let rightArrow = (await driver.findElements(By.className('right-arrow')))[0];
  let questionInputs = (await driver.findElements(By.css('input[type=text]')));

  for (const qInput of questionInputs) {
    await qInput.sendKeys(String(Math.floor(Math.random() * 10)));
    await rightArrow.click();
  }

  let stopButton = (await driver.findElements(By.className('stop-button')))[0];
  expect(await stopButton.isEnabled()).to.be.true;
  return stopButton.click();
}

describe('user', function () {
  let driver: ThenableWebDriver;
  let secondDriver: ThenableWebDriver;

  it('should be logged out when the user changes their password', async function() {
    this.timeout(50000);

    driver = new Builder().forBrowser('firefox').build();

    await maybeLogIn(driver, 'user1');

    // Zapisujemy ciasteczka z pierwszej sesji
    let cookies = await driver.manage().getCookies();
    await driver.manage().deleteAllCookies();

    // Otwieramy drugą sesję
    secondDriver = new Builder().forBrowser('firefox').build();
    await maybeLogIn(secondDriver, 'user1');

    // Zmieniamy hasło
    await secondDriver.get(addrBase + 'change-password');

    await secondDriver.findElement(By.id('password')).sendKeys('user3');
    await secondDriver.findElement(By.css('input[type=submit]')).click();

    await secondDriver.wait(until.elementLocated(By.id('username')));

    // Dodajemy ciasteczka z pierwszej sesji
    for (const cookie of cookies) {
      await secondDriver.manage().addCookie(cookie);
    }

    // Sprawdzamy czy rzeczywiście wylogowało wszystkie sesje użytkownika
    await secondDriver.get(addrBase);
    expect (await secondDriver.findElements(By.className('logged-in-as'))).to.be.empty;
  });

  after(async function() {
    this.timeout(50000);

    await maybeLogIn(secondDriver, 'user3');
    await secondDriver.get(addrBase + 'change-password');

    // Przywracamy stare hasło
    await secondDriver.findElement(By.id('password')).sendKeys('user1');
    await secondDriver.findElement(By.css('input[type=submit]')).click();
    await secondDriver.wait(until.elementLocated(By.id('username')));

    await secondDriver.close();

    return driver.close();
  });
});

describe('json', function() {
  let driver: ThenableWebDriver;

  it('with the quiz should be received after pressing start', async function() {
    this.timeout(50000);

    driver = new Builder().forBrowser('firefox').build();

    await maybeLogIn(driver, 'user1');

    await driver.wait(until.elementsLocated(By.css('option')));

    // Sprawdzamy czy otrzymano JSONa z listą quizów
    let performanceEntries =
          await driver.executeScript(`return window
                                            .performance
                                            .getEntriesByName("http://127.0.0.1:3000/api/unsolved-quizzes");`) as Array<any>;

    expect(performanceEntries).to.not.be.empty;

    await (await driver.findElements(By.className('start-button')))[0].click();
    await driver.wait(until.elementsLocated(By.css('input[type=text]')));

    // Sprawdzamy, czy otrzymano JSON z quizem od serwera
    performanceEntries = await driver.executeScript('return window.performance.getEntries();') as Array<any>;
    let quizJsonReceived: boolean = false;

    for (const entry of performanceEntries) {
      if (entry.initiatorType === "fetch" &&
          (entry.name as string).match(/^http:\/\/127\.0\.0\.1:3000\/api\/quizzes\/(\d)+$/) &&
          entry.decodedBodySize > 0) {
        quizJsonReceived = true;
      }
    }
    
    expect(quizJsonReceived).to.be.true;
  });

  it('with the responses should be sent after solving the quiz and pressing stop', async function() {
    this.timeout(50000);

    await solveQuiz(driver);
    await driver.wait(until.urlIs(addrBase + 'results'));

    // Pola z wynikami istnieją, więc serwer pomyślnie odebrał od nas JSON i zapisał wyniki
    expect(await driver.findElements(By.className('response-stats'))).to.not.be.empty;
  });

  after(async function() {
    this.timeout(50000);

    return driver.close();
  });
});

describe('quiz', function() {
  let firstDriver: ThenableWebDriver;
  let secondDriver: ThenableWebDriver;

  it('should not be solvable after being solved once', async function() {
    this.timeout(50000);

    firstDriver = new Builder().forBrowser('firefox').build();
    secondDriver = new Builder().forBrowser('firefox').build();

    // W dwóch sesjach logujemy się jako user1
    await maybeLogIn(firstDriver, 'user1');
    await maybeLogIn(secondDriver, 'user1');

    // Pierwszy rozwiązuje quiz
    await (await firstDriver.findElements(By.className('start-button')))[0].click();
    await firstDriver.wait(until.elementsLocated(By.css('input[type=text]')));
    await solveQuiz(firstDriver);

    // Sprawdzamy czy drugi już nie może rozwiązać, mimo że wyświetla mu się jeszcze ten quiz jako dostępny.
    await (await secondDriver.findElements(By.className('start-button')))[0].click();

    expect(await secondDriver.findElement(By.id('error-message')).getText())
            .to.have.string('Wybrany quiz jest już rozwiązany. Odśwież stronę, by zobaczyć aktualną listę quizów.');
  });

  after(async function() {
    this.timeout(50000);

    await firstDriver.close();
    return secondDriver.close();
  });
});