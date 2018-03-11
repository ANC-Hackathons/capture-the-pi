let express = require('express')
let app = express()

let blue= 0;
let red= 0;

app.get('/', function (req, res) {
    res.send('Hello World')
})

app.get('/score', function(req, res) {
  res.json({ red, blue });
});

app.get('/score/red', function(req, res) {
  res.json({ red });
});

app.get('/score/blue', function(req, res) {
  res.json({ blue });
});

app.put('/score/red/:score', function(req, res) {
  red += req.params.score;
  console.log(`Red score now: ${red}`);
}

app.put('/score/blue/:score', function(req, res) {
  blue += req.params.score;
  console.log(`Blue score now: ${blue}`);
}

app.post('/reset', function(req, res) {
  red = 0;
  blue = 0;
  console.log('Reset occurred');
  console.log(`Red score now: ${red}`);
  console.log(`Blue score now: ${blue}`);
}

app.listen(3000)
