var createError = require('http-errors');
var express = require('express');
var path = require('path');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var session = require('express-session');
var SQLiteStore = require('connect-sqlite3')(session);
const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;

const { quizDB } = require('./dist/server/db-interaction');
const { msToSecString } = require('./dist/utils');

passport.use(new LocalStrategy(async function(username, password, done) {
  try {
    let user = await quizDB.getPlayer(username, 'username');

    if (!user)
      return done(null, false);

    if (!user.validPassword(password))
      return done(null, false);

    return done(null, user);
  } catch (err) {
    return done(err);
  }
}));

passport.serializeUser(function(user, done) {
  done(null, user.id);
});

passport.deserializeUser(async function(id,  done) {
  try{
    let user = await quizDB.getPlayer(id, 'id');
    done(null, user);
  } catch(err) {
    done(err);
  }
});

var indexRouter = require('./routes/index');
var statsRouter = require('./routes/stats');
var apiRouter = require('./routes/api');

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser('da423v89axc32q1'));
app.use(express.static(path.join(__dirname, 'public')));

app.use(session({
  secret: 'da423v89axc32q1',
  resave: false,
  saveUninitialized: false,
  store: new SQLiteStore({
    table: 'sessions',
    db: quizDB.name,
    dir: __dirname
  })
}));

app.use(passport.initialize());
app.use(passport.session());

app.use('/', indexRouter);
app.use('/stats', statsRouter);
app.use('/api', apiRouter);

app.locals.msToSec = msToSecString;

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;
