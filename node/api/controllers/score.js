let blue= 0;
let red= 0;
let killedRecently = false;

function getFullScore(req, res) {
  res.json({ red, blue, killedRecently });
}

function getRedScore(req, res) {
  res.json({ red });
}

function putRedScore(req, res) {
  red += parseInt(req.swagger.params.score.value, 10);
  console.log(`Red score now: ${red}`);
  res.sendStatus(200);
}

function getBlueScore(req, res) {
  res.json({ blue });
}

function putBlueScore(req, res) {
  console.log(req.swagger.params.score.value);
  blue += parseInt(req.swagger.params.score.value, 10);
  console.log(`Blue score now: ${blue}`);
  res.sendStatus(200);
}

function resetScore(req, res) {
  red = 0;
  blue = 0;
  console.log('Reset occurred');
  console.log(`Red score now: ${red}`);
  console.log(`Blue score now: ${blue}`);
  res.sendStatus(200);
}

function newKill(req, res) {
  killedRecently = true;
  setTimeout(function() {
    killedRecently = false;
  }, 10000);
  res.sendStatus(200);
}

module.exports = {
  getFullScore,
  getRedScore,
  putRedScore,
  getBlueScore,
  putBlueScore,
  resetScore,
  newKill
};
