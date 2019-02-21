const { expect } = require("chai");
const { test } = require("./lib");
const Path = require("path");
const { promisify } = require("util");
const fs = require("fs");
const { exec } = require('child_process');

describe("Syntax", () =>
{
    it("BNF analyse test", async () =>
    {
        const result = (await promisify(exec)(`"./test/build/test-bnf" "${Path.resolve("./test/bnf-test.bnf")}"`)).stdout;
        const expectResult = (await promisify(fs.readFile)(Path.resolve("./test/bnf-result.bnf"))).toString();
        expect(result).be.equal(expectResult);
    });
});