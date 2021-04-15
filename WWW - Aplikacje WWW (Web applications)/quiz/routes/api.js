var express = require('express');
var router = express.Router();
var createError = require('http-errors');

const { checkIfLoggedIn } = require('../middleware');
const { quizDB } = require('../dist/server/db-interaction');

router.get('/unsolved-quizzes', checkIfLoggedIn, async function(req, res, next) {
  if (!req.isAuthenticated())
    return res.redirect('/');

  try {
    let quizzes = await quizDB.getQuizList(req.user, false);
    let topFives = await quizDB.getTopFives(quizzes);

    res.json({
      quizzes: quizzes,
      top_fives: topFives
    })
  } catch (e) {
    console.log(e);
    next();
  }
});

router.get('/quizzes/:quizID', async function(req, res, next) {
  try {
    if (!req.user) {
      return res.json({
        error: "Nie jesteś zalogowany."
      });
    }
    let solved = await quizDB.getQuizList(req.user, true);
    for (const solvedQuiz of solved) {
      if (solvedQuiz.id == req.params.quizID) {
        return res.json({
          error: "Wybrany quiz jest już rozwiązany. Odśwież stronę, by zobaczyć aktualną listę quizów."
        });
      }
    }

    let quiz = await quizDB.getQuiz(req.params.quizID);
    req.session.quizSendTime = Date.now();

    res.json(quiz);
  } catch (e) {
    console.log(e);
    next();
  }
});

module.exports = router;