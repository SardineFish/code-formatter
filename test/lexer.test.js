const { expect } = require("chai");
const { test } = require("./lib");
const Path = require("path");
const { promisify } = require("util");
const fs = require("fs");
const { exec } = require('child_process');

describe("Lexer", () =>
{
    it("C code test", async () =>
    {
        //const result = await test("./test/build/test-lexer", [Path.resolve("./test/test.code")]);
        const result = (await promisify(exec)(`"./test/build/test-lexer" "${Path.resolve("./test/code-test.c")}"`)).stdout;
        const expectResult = (await promisify(fs.readFile)(Path.resolve("./test/lex-result.txt"))).toString();
        expect(result).be.equal(expectResult);
    });
});