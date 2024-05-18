let sentence;

const answerContainer = document.getElementById("answer-field");
const tileContainer = document.getElementById("tile-field");

const shuffleSentence = (sentence) => {
  const split = sentence.trim().split(" ").filter((w) => w !== "");

  let m = split.length;
  let t;
  let i;

  while (m) {
      i = Math.floor(Math.random() * m--);
      t = split[m];
      split[m] = split[i];
      split[i] = t;
  }

  return split;
}

const clearTiles = () => {
    const clear = (container) => {
        while(container.firstChild) {
            container.removeChild(container.lastChild);
        }
    }

    clear(answerContainer);
    clear(tileContainer);
}

const createTile = (word, container = tileContainer) => {
    let tile = document.createElement("div");
    tile.classList.add("col");
    tile.classList.add("tile");
    tile.innerHTML = word;
    container.appendChild(tile);
}

const populateTiles = (sentence, container = tileContainer) => {
    shuffleSentence(sentence).forEach((w) => createTile(w, container));
}

const handleCorrect = () => {
    console.log("CORRECT!");
    for (let tile of document.getElementsByClassName("tile")) {
        tile.classList.add("correct-tile");
    }
    fetch("/api/reward", {method: 'POST'}).then(() => console.log("Gave a reward!"));
}

const handleStop = () => {
    fetch("/api/stop", {method: 'POST'}).then(() => console.log("Stopped the chair!"));
}

const checkSentence = (correct) => {
    let solution = "";
    for (const tile of answerContainer.children) {
        solution += tile.innerHTML;
    }
    if (solution === correct.replaceAll(" ", "")) {
        handleCorrect();
        return true;
    }
    console.log(solution);
    return false;
}

const setupTask = async () => {
    const response = await fetch('/api/sentence');
    sentence = await response.text();
    clearTiles();
    populateTiles(sentence);
}

const allowDrag = (el, source, handle, sibling) => {
    if (handle.classList.contains("correct-tile")) return false;
    return true;
}

const acceptDrag = (el, source, target, sibling) => {
    if (target.id === "answer-field" && source.id === "tile-field") return false;
    return true;
}

const drake = dragula([tileContainer, answerContainer], {
        moves: allowDrag,
        accepts: acceptDrag
    }
);

drake.on("drop", () => checkSentence(sentence));
setupTask();