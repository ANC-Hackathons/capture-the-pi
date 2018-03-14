let express = require('express')
let app = express()
let port = process.env.PORT || 8080

let blue= 0;
let red= 0;
let killedRecently = false;

app.get('/', function (req, res) {
    res.send('Hello World')
})

app.get('/score', function(req, res) {
  res.json({ red, blue, killedRecently });
});

app.get('/score/red', function(req, res) {
  res.json({ red });
});

app.get('/score/blue', function(req, res) {
  res.json({ blue });
});

app.put('/score/red/:score', function(req, res) {
  red += parseInt(req.params.score, 10);
  console.log(`Red score now: ${red}`);
  res.sendStatus(200);
});

app.put('/score/blue/:score', function(req, res) {
  blue += parseInt(req.params.score);
  console.log(`Blue score now: ${blue}`);
  res.sendStatus(200);
});

app.post('/reset', function(req, res) {
  red = 0;
  blue = 0;
  console.log('Reset occurred');
  console.log(`Red score now: ${red}`);
  console.log(`Blue score now: ${blue}`);
  res.sendStatus(200);
});

app.post('/newKill', function(req, res) {
  killedRecently = true;
  setTimeout(function() {
    killedRecently = false;
  }, 10000);
  res.sendStatus(200);
});

console.log(`Listening on port: ${port}`);
app.listen(port)
