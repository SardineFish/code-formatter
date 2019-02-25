const spawn = require("child_process").spawn;
const promisify = require("util").promisify;
const expect = require("chai").expect;
const {
    test,
    asyncForEach
} = require("./lib");

async function testRegExp(case_, greedy)
{
    await asyncForEach(case_.samples, async (sample) =>
    {
        const result = await runTest("./test/build/test-regexp", greedy?["--greedy"]:[], case_.regex + "\0" + sample + "\0");
        const expectResult = case_.regexJS.exec(sample);
        if (expectResult == null)
            expect(result, `sample '${sample}'`).be.equal("null");
        else
            expect(result.replace(/\r/g, '\\r').replace(/\n/g, '\\n'), `sample '${sample}'`).be.equal(case_.regexJS.exec(sample)[0].replace(/\r/g, '\\r').replace(/\n/g, '\\n'));
    });
}
function runTest(path, args, input) {
    return new Promise((resolve) => {
        const child = spawn(`${path}`, args);
        child.stdin.write(input);
        child.stdin.end();
        child.stdout.on("data", (data) => {
            resolve(data.toString());
        });
    });
}

describe("Testing RegExp", () =>
{
    it("Numbers", async () =>
    {
        const testCase = {
            regex: "(0x[0-9A-Fa-f]+)|((\\d)+)((\\.((\\d)+))?)((e(\\+|-)?((\\d)+))?)",
            regexJS: /^((0x[0-9A-Fa-f]+)|(((\d)+)((\.((\d)+))?)((e(\+|-)?((\d)+))?)))/,
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
        };
        await testRegExp(testCase, true);
    });

    it("Operator", async () =>
    {
        const testCase = {
            regex: "(->)|(\\+\\+|--)|(\\|\\||&&)|((\\+|-|\\*|/|%|=|&|\\||\\^|<<|>>|<|>|=|!)=?)|(\\?|:|,|\\.)",
            regexJS: /^((->)|(\+\+|--)|(\|\||&&)|((\+|-|\*|\/|%|=|&|\||\^|<<|>>|<|>|=|!)=?)|(\?|:|,|\.))/,
            samples: [
                "1e-9",
                "-",
                "++",
                "--",
                "==",
                "!=",
                "?",
                ":",
                ">=",
                "&=",
                "%=",
                "|",
                "!",
                "^",
                ">>",
                ">>=",
                "++5",
                "-6",
                ",",
                ".",
                "*a",
                "&a",
                "->",
            ]
        };
        await testRegExp(testCase, true);
    });

    it("Char", async () => {
        const testCase = {
            regex: "'(\\\\(\\S|\\d+)|.)'",
            regexJS: /^'(\\(\S|\d+)|.)'/,
            samples: [
                '"Hello World!\\n"',
                "'a'",
                "'0'",
                "'\\0'",
                "'\\n'",
                "' '",
                "''"
            ]
        };
        await testRegExp(testCase, false);
    });

    it("Strings", async () => {
        const testCase = {
            regex: '"([^\\"]|\\\\\\S|\\")*"',
            regexJS: /^("([^\\"]|\\\S|\\")*")/,
            samples: [
                '"Hello World!\\n"',
                '"the boy next door."',
                '"triple " quote"',
                '"escape\\n\\r\\b\\v\\t\\"\\\'"',
                '"newline \r\n"',
                '"mac \r"',
                //'"linux \n"',
            ]
        };
        await testRegExp(testCase, false);
    });

    it("Identifier", async () =>
    {
        const testCase = {
            regex: '[a-zA-Z_][a-zA-Z0-9]*',
            regexJS: /^([a-zA-Z_][a-zA-Z0-9]*)/,
            samples: [
                'the boy next door.',
                '1145141919',
                '_114514',
                '_A5s_we_can',
                "______________________"
            ]
        };
        await testRegExp(testCase, true);
    });

    it("Comment", async () => {
        const testCase = {
            regex: '//.*\r\n|/\\*.*\\*/',
            regexJS: /^((\/\*.*\*\/)|(\/\/.*\r\n))/,
            samples: [
                '//23333\r\n333',
                '/*the boy next door.*/',
                '/*********/',
                '// /*23333*/ \r\n',
                '/* // _A5s_we_can\r\n*/',
                "/* * /* ** * **/"
            ]
        };
        await testRegExp(testCase, false);
    });

    it("Key words", async () => {
        const testCase = {
            regex: '#include|#define|void|char|short|int|long|unsigned|double|float|if|else if|else|for|while|do|break|continue|return|switch|case',
            regexJS: /#include|#define|void|char|short|int|long|unsigned|double|float|if|else if|else|for|while|do|break|continue|return|switch|case/,
            samples: [
                "#include",
                "#define",
                "void",
                "char",
                "short",
                "int",
                "long",
                "unsigned",
                "double",
                "float",
                "if",
                "else if",
                "else",
                "for",
                "while",
                "do",
                "break",
                "continue",
                "return",
                "switch",
                "case",
            ]
        };
        await testRegExp(testCase, true);
    });
});