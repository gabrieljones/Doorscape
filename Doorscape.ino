bool amDoor = false;

byte doorCombo[6] = {4, 4, 4, 4, 4, 4};
Color cardColors[9] = {dim(WHITE, 50), RED, YELLOW, GREEN, BLUE};

enum protoc {EMPTY, REDCARD, YELCARD, GRECARD, BLUCARD, WON};
byte deckContents[13] = {REDCARD, YELCARD, GRECARD, BLUCARD, REDCARD, YELCARD, GRECARD, BLUCARD, REDCARD, YELCARD, GRECARD, BLUCARD, EMPTY};
byte deckPosition = 0;

byte score = 0;

byte oldCorrect = 0;

Timer doorTimer;
#define DOOR_CODE_TIME 20000

Timer drawAnimTimer;
#define DRAW_ANIM_TIME 400

#define SCORE_TO_WIN 6
//#define SCORE_TO_WIN 1

bool isSolved = false;

void setup() {
  // put your setup code here, to run once:
  randomize();
  newDoor();
  shuffleDeck();
}

void loop() {

  if (amDoor) {
    if (score < SCORE_TO_WIN) {
      doorLoop();
    } else {
      winLoop();
    }
  } else {
    cardLoop();
    cardDisplay();
  }

}

void doorLoop() {
  if (buttonMultiClicked()) {
    if (buttonClickCount() == 3 ) {
      amDoor = false;
    }
  }

  //look at neighbors, determine if the combo has been made
  byte correctNeighbors = 0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//neighbor!
      if (doorCombo[f] > EMPTY) {//this face WANTS a neighbor
        byte neighborColor = getCardColor(getLastValueReceivedOnFace(f));
        if (neighborColor == doorCombo[f]) {
          setColorOnFace(WHITE, f);
          correctNeighbors++;
        } else {
          setColorOnFace(cardColors[doorCombo[f]], f);
        }
      }
    } else {//no neighbor!
      if (doorCombo[f] == EMPTY) {//this face wants NO NEIGHBOR
        correctNeighbors++;
      }
      setColorOnFace(cardColors[doorCombo[f]], f);
    }
  }//end of neighborhood check loop
  
  
  if (oldCorrect != correctNeighbors) {
    console.log(`${oldCorrect} >>> ${correctNeighbors}`);
    oldCorrect = correctNeighbors;
  }

  if (correctNeighbors == 5) {
    isSolved = true;
  }

  if (isSolved) {
    setColor(WHITE);
    setColorOnFace(RED, random(5));
    setColorOnFace(YELLOW, random(5));
    setColorOnFace(GREEN, random(5));
    setColorOnFace(BLUE, random(5));
    score++;
    newDoor();
  } else {
    setValueSentOnAllFaces(EMPTY);
    //FOREACH_FACE(f) {
      //setColorOnFace(cardColors[doorCombo[f]], f);
    //}
  }

}

void newDoor() {
    //randomize door code
    FOREACH_FACE(f) {
      doorCombo[f] = random(4);
    }
    //reset isSolved
    isSolved = false;
}

void winLoop() {
  if (buttonSingleClicked()) {
    isSolved = false;
    score = 0;
  }
  setColor(WHITE);
  setColorOnFace(RED, random(5));
  setColorOnFace(YELLOW, random(5));
  setColorOnFace(GREEN, random(5));
  setColorOnFace(BLUE, random(5));
  setValueSentOnAllFaces(WON);
}

void cardLoop() {
  if (buttonSingleClicked()) {
    if (deckPosition < 12) {
      deckPosition++;
      drawAnimTimer.set(DRAW_ANIM_TIME);
    } else {
      //BAD TIMES YO
    }
  }

  if (buttonDoubleClicked()) {
    deckPosition = 0;
    shuffleDeck();
  }

  if (buttonMultiClicked()) {
    if (buttonClickCount() == 3 ) {
      amDoor = true;
      score = SCORE_TO_WIN;
      isSolved = true;
    }
  }
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//neighbor!
      if (getLastValueReceivedOnFace(f) == WON) {
        deckPosition = 0;
        shuffleDeck();
      }
    }
  }

  byte sendData = (amDoor << 4) | (deckContents[deckPosition]);
  setValueSentOnAllFaces(sendData);
}

void cardDisplay() {
  //am I idle, or am I drawing?
  if (drawAnimTimer.isExpired()) {//just sitting idly showing cards
    setColor(cardColors[deckContents[deckPosition]]);
    if (deckPosition < 12) {
      setColorOnFace(cardColors[deckContents[deckPosition + 1]], 0);
    }
  } else {//doing draw animation
    byte currentFrame = map(DRAW_ANIM_TIME - drawAnimTimer.getRemaining(), 0, DRAW_ANIM_TIME, 1, 3);
    switch (currentFrame) {
      case 1:
        setColor(cardColors[deckContents[deckPosition - 1]]);
        setColorOnFace(cardColors[deckContents[deckPosition]], 0);
        break;
      case 2:
        setColor(cardColors[deckContents[deckPosition - 1]]);
        setColorOnFace(cardColors[deckContents[deckPosition]], 0);
        setColorOnFace(cardColors[deckContents[deckPosition]], 1);
        setColorOnFace(cardColors[deckContents[deckPosition]], 5);
        break;
      case 3:
        setColor(cardColors[deckContents[deckPosition]]);
        setColorOnFace(cardColors[deckContents[deckPosition - 1]], 3);
        break;
    }
  }
}

void shuffleDeck() {
  for (byte i = 0; i < 30; i++) {
    byte swapA = random(11);
    byte swapB = random(11);
    byte temp = deckContents[swapA];
    deckContents[swapA] = deckContents[swapB];
    deckContents[swapB] = temp;
  }
}

byte getAmDoor(byte data) {
  return (data >> 4);
}

byte getCardColor(byte data) {
  return (data & 15);
}
