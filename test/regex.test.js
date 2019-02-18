const exec = require("child_process").exec;
const promisify = require("util").promisify;
const expect = require("chai").expect;
const {
    test,
    asyncForEach
} = require("./lib");

describe("Testing RegExp", () => {
    const cases = [{
        regex: "((\\d)+)((\\.((\\d)+))?)((e(\\+|-)?((\\d)+))?)",
        regexJS: /((\d)+)((\.((\d)+))?)((e(\+|-)?((\d)+))?)/,
        samples: [
            "0.9",
            "9",
            "12345678900987654321",
            "114514.1919",
            "000023333.23333",
            "0x8000",
            "0x0",
            "0x1234567890ABCDEF",
            "1e5-9",
            "1e9.9",
            "0e-4",
            "00.5e6"
        ]
    }];
    it("Numbers", async () => {
        await asyncForEach(cases, async (cs) => {
            await asyncForEach(cs.samples, async (sample) => {
                const result = await test("test/build/test-regexp", ["--greedy", cs.regex, sample]);
                expect(result).be.equal(cs.regexJS.exec(sample)[0]);
            })
        })
        const result = await test("test/build/test-regexp", ['abc', "abc"]);
        expect(result).to.be.equal("abc");
    });
});