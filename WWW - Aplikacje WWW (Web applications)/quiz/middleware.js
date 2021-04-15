var express = require('express');
var csurf = require('csurf');

var csrfProtection = csurf({ cookie: true });

var checkIfLoggedIn = function(req, res, next) {
  if (req.isUnauthenticated())
    return res.redirect('/');

  res.locals.username = req.user.username;
  
  next();
}

module.exports = { csrfProtection, checkIfLoggedIn };
