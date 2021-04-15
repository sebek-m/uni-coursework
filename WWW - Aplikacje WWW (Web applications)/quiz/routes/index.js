var express = require('express');
var router = express.Router();
var createError = require('http-errors');
const passport = require('passport');

const { csrfProtection, checkIfLoggedIn } = require('../middleware');
const { quizDB } = require('../dist/server/db-interaction');

router.get('/', async function(req, res, next) {
  if (!req.isAuthenticated()) {
    if (req.session.messages) {
      req.session.messages = null;
      res.locals.login_failed = true;
    }
    return res.render('login');
  }

  res.locals.username = req.user.username;

  try {
    let quizCount = await quizDB.getUnsolvedQuizCount(req.user);
    await quizDB.getTopFive(1);

    res.render('main', { quiz_count: quizCount });
  } catch (e) {
    next(createError(e));
  }
});

router.post('/submit-responses', checkIfLoggedIn, async function(req, res, next) {
  let solutionTime = Date.now() - req.session.quizSendTime;

  try {
    await quizDB.consumeResponses(req.body.quizID, req.body.responses, req.user, solutionTime);
    req.session.quizID = req.body.quizID;

    res.end();
  } catch (e) {
    console.log(e);
    next(createError(e));
  }
});

router.get('/results', checkIfLoggedIn, async function(req, res, next) {
  try {
    if (!req.session.quizID)
      res.redirect('/');

    let stats = await quizDB.getStats(req.user, req.session.quizID);
    let meta = await quizDB.getQuizMeta(req.session.quizID);

    req.session.quizID = null;

    res.render('results', { stats: stats, meta: meta });
  } catch (e) {
    console.log(e);
    next(createError(e));
  }
});

router.post('/login', passport.authenticate('local', { successRedirect: '/',
                                                        failureRedirect: '/',
                                                        failureMessage: 'fail'}));

router.get('/change-password', checkIfLoggedIn, csrfProtection, function(req, res) {
  res.render('change-password', { username: req.user.username, csrfToken: req.csrfToken() });
});

router.post('/change-password', csrfProtection, async function(req, res, next) {
  try {
    await quizDB.changePassword(req.user, req.body.password);
    let player = req.user;

    req.logOut();
    await quizDB.logoutPlayerSessions(player);

    res.redirect('/');
  } catch (e) {
    next(createError(e));
  }
});

router.get('/logout', function(req, res) {
  req.logOut();
  res.redirect('/');
});

module.exports = router;
