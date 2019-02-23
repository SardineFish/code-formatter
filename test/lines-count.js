const fs = require("fs");
const Path = require("path");
const { promisify } = require("util");
const linq = require("linq");

let total = 0;
console.log(
    linq.from(fs.readdirSync("./src"))
        .select(file => Path.resolve("./src", file))
        .sum(path => fs.readFileSync(path).toString().split("\r\n").length));
