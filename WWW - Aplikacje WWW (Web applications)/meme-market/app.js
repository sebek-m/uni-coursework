var createError = require('http-errors');
var express = require('express');
var path = require('path');
var logger = require('morgan');
var csurf = require('csurf');
var cookieParser = require('cookie-parser');
var session = require('express-session');
var SQLiteStore = require('connect-sqlite3')(session);
const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;

const { memeDB } = require('./build/meme-db-instance');
const { get_meme } = require('./build/meme-functions');

passport.use(new LocalStrategy(async function(username, password, done) {
  try {
    let user = await memeDB.getUser(username, 'username');

    if (!user) {
      user = await memeDB.addUser(username, password);
      return done(null, user);
    }

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
    let user = await memeDB.getUser(id, 'id');
    done(null, user);
  } catch(err) {
    done(err);
  }
});

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

app.use(logger('dev'));
app.use(express.urlencoded({ extended: true }));
app.use(express.static(path.join(__dirname, 'public')));
app.locals.basedir = path.join(__dirname, 'public');

/*
  https://www.npmjs.com/package/express-session - "Using cookie-parser may result in issues
  if the secret is not the same between this module and cookie-parser." a potrzebujemy
  cookie-parsera do csurfa, więc ustawiamy taki klucz.
*/
app.use(cookieParser('tajny klucz do meme'));
var csrfProtection = csurf({ cookie: true });

app.use(session({
  secret: 'tajny klucz do meme',
  resave: true,
  saveUninitialized: false,
  cookie: {
    maxAge: 15 * 60 * 1000
  },
  store: new SQLiteStore({
    table: 'sessions',
    db: 'meme_db.db',
    dir: __dirname
  })
}));

app.use(passport.initialize());
app.use(passport.session());

/*
  Handler aktualizujący liczbę odwiedzin użytkownika.
  Nie ustawiamy go jako middleware przy pomocy use,
  żeby uniknąć zwiększenia licznika przez żądania
  o favicon.
*/
function increaseViewCount(req, res, next) {
  if (req.session.page_views) {
    req.session.page_views++;
    res.locals.page_s = 'pages';
  }
  else {
    req.session.page_views = 1;
    res.locals.page_s = 'page';
  }

  res.locals.view_count = req.session.page_views;
  
  next();
}

function setAuthenticationLocals(req, res, next) {
  if (req.isAuthenticated()) {
    res.locals.username = req.user.username;
  }
  else if (req.session.messages) {
    req.session.messages = null;
    res.locals.login_failed = true;
  }

  next();
}

/* Trasy ze specyfikacji */

app.get('/', increaseViewCount, setAuthenticationLocals, async function(req, res) {
  let topMemes;

  try {
    topMemes = await memeDB.getThreeMostExpensive();
  } catch (err) {
    return next(err);
  }

  res.render('index', { title: 'Meme market',
                        message: 'Hello there!',
                        memes: topMemes});
});

app.get('/meme/:memeId', increaseViewCount, setAuthenticationLocals, csrfProtection, async function(req, res) {
  let meme;

  try {
    meme = await get_meme(req.params.memeId);
  } catch (err) {
    return next(err);
  }

  if (!meme)
    return res.status(404).send('Meme not found!');

  res.render('meme', { meme: meme, csrfToken: req.csrfToken() });
});

app.post('/meme/:memeId', increaseViewCount, setAuthenticationLocals, csrfProtection, async function(req, res) {
  if (req.isUnauthenticated()) {
    return next("Unexpected error");
  }

  let meme;

  try {
    meme = await get_meme(req.params.memeId);
  } catch (err) {
    return next(err);
  }

  if (!meme) {
    res.status(404).send('Meme not found!');
  }
  else {
    let price = req.body.price;

    if (price == "") {
      res.render('meme', { valid: 'n',
                          message: 'Please give a valid price',
                          meme: meme, csrfToken: req.csrfToken() });
    }
    else {
      // W celu usunięcia ewentualnych zer wiodących
      price = parseInt(price, 10);
      try {
        await meme.change_price(price, req.user);
      } catch(err) {
        return next(err);
      }

      res.render('meme', { valid: 'y',
                          message: 'Changed price successfully',
                          meme: meme, csrfToken: req.csrfToken() });
    }
  }
});

app.post('/login', passport.authenticate('local', { successRedirect: 'back',
                                                    failureRedirect: 'back',
                                                    failureMessage: 'fail'}));

app.get('/logout', function(req, res) {
  req.logOut();
  res.redirect('back');
})

/* Koniec tras ze specyfikacji */

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // send error
  res.status(err.status || 500);
  res.send('ERROR: ' + err.message);
});

module.exports = app;
