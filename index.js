const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");

class Vector2 {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }

  *xy() {
    yield this.x;
    yield this.y;
  }

  distSqr(other) {
    const dx = this.x - other.x;
    const dy = this.y - other.y;
    return dx * dx + dy * dy;
  }
}

function vec2(x, y) {
  return new Vector2(x, y);
}

class Circle {
  constructor() {
    this.left = null;
    this.right = null;
    this.degree = 1;
  }

  draw(position, radius) {
    ctx.save();
    ctx.beginPath();
    const leftPos = vec2(position.x - radius / 2, position.y);
    const rightPos = vec2(position.x + radius / 2, position.y);
    if (this.left) this.left.draw(leftPos, radius / 2);
    if (this.right) this.right.draw(rightPos, radius / 2);
    ctx.arc(...position.xy(), radius, 0, Math.PI * 2);
    ctx.closePath();
    ctx.stroke();
    ctx.textAlign = "center";
    ctx.fillText(this.degree, position.x, position.y + radius / 2, 2 * radius);
    ctx.restore();
  }

  balance(direction) {
    if (!this.left || !this.right) return;
    this.left.balance("right");
    this.right.balance("left");

    if (this.left.degree - this.right.degree > 1) {
      this.left.takeOne();
      this.right.pushOne();
    } else if (this.right.degree - this.left.degree > 1) {
      this.right.takeOne();
      this.left.pushOne();
    }

    if (this.degree % 2 == 1) {
      if (direction === "right" && this.left.degree > this.right.degree) {
        this.left.takeOne();
        this.right.pushOne();
      } else if (direction === "left" && this.right.degree > this.left.degree) {
        this.right.takeOne();
        this.left.pushOne();
      }
    }
  }

  takeOne() {
    if (this.degree == 2) {
      this.left = null;
      this.right = null;
    } else if (this.left.degree > this.right.degree) {
      this.left.takeOne();
    } else {
      this.right.takeOne();
    }
    this.computeDegree();
  }

  pushOne() {
    if (!this.left || !this.right) {
      this.left = new Circle();
      this.right = new Circle();
    } else if (this.left.degree < this.right.degree) {
      this.left.pushOne();
    } else {
      this.right.pushOne();
    }
    this.computeDegree();
  }

  subdivideAt(position, radius, at) {
    const leftPos = vec2(position.x - radius / 2, position.y);
    const rightPos = vec2(position.x + radius / 2, position.y);
    if (this.left && leftPos.distSqr(at) < ((radius / 2) * radius) / 2) {
      this.left.subdivideAt(leftPos, radius / 2, at);
    } else if (
      this.right &&
      rightPos.distSqr(at) < ((radius / 2) * radius) / 2
    ) {
      this.right.subdivideAt(rightPos, radius / 2, at);
    } else if (this.left || this.right) {
      console.log("Can't subdivide subdivided circle");
    } else {
      this.left = new Circle();
      this.right = new Circle();
    }
    this.computeDegree();
  }

  computeDegree() {
    if (this.left && this.right)
      this.degree = this.left.degree + this.right.degree;
    else this.degree = 1;
  }
}

let circle = new Circle();

function getMousePosition(canvas, event) {
  const rect = canvas.getBoundingClientRect();
  const x = event.clientX - rect.left;
  const y = event.clientY - rect.top;
  return vec2(x, y);
}

canvas.addEventListener("mousedown", function (event) {
  circle.subdivideAt(
    vec2(canvas.width / 2, canvas.height / 2),
    (canvas.width / 2) * 0.9,
    getMousePosition(canvas, event),
  );
});

function balance() {
  circle.balance("root");
}

function init() {
  window.requestAnimationFrame(draw);
}

function draw() {
  if (
    canvas.clientWidth != canvas.width ||
    canvas.clientHeight != canvas.height
  ) {
    canvas.width = canvas.clientWidth;
    canvas.height = canvas.clientHeight;
  }
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  ctx.lineWidth = 1;
  ctx.strokeStyle = "black";
  circle.draw(
    vec2(canvas.width / 2, canvas.height / 2),
    (canvas.width / 2) * 0.9,
  );

  window.requestAnimationFrame(draw);
}

init();
