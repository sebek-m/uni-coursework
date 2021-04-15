var express = require('express');
var router = express.Router();
var createError = require('http-errors');

const { checkIfLoggedIn } = require('../middleware');
const { quizDB } = require('../dist/server/db-interaction');

router.get('/', checkIfLoggedIn, async function(req, res, next) {
  try {
    let solvedQuizzes = await quizDB.getQuizList(req.user, true);

    res.render('stats-main', { solved_quizzes: solvedQuizzes});
  }
  catch (e) {
    next(createError(e));
  }
});

router.get('/:quizID', checkIfLoggedIn, async function(req, res, next) {
  try {
    let stats = await quizDB.getStats(req.user, req.params.quizID);
    let meta = await quizDB.getQuizMeta(req.params.quizID);

    res.render('stats-details', { stats: stats, meta: meta});
  }
  catch (e) {
    next(createError(e));
  }
});

module.exports = router;
