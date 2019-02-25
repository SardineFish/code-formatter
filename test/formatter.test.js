const { expect } = require("chai");
const { test } = require("./lib");
const Path = require("path");
const { promisify } = require("util");
const fs = require("fs");
const { exec } = require('child_process');

describe("Syntax", () =>
{
    it("Format test", async () =>
    {
        const result = (await promisify(exec)(`"./test/build/test-formatter"`)).stdout;
        fs.writeFileSync("./test/format-result.c", result);
        const originLex = (await promisify(exec)(`"./test/build/test-lexer" "${Path.resolve("./test/code-test.c")}"`)).stdout;
        const formatedLex = (await promisify(exec)(`"./test/build/test-lexer" "${Path.resolve("./test/format-result.c")}"`)).stdout;
        expect(formatedLex).equal(formatedLex);
    });
});