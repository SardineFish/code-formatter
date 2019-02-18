const {
    exec,
    spawn
} = require("child_process");
const promisify = require("util").promisify;

/*
describe("233", () =>
{
    it("t", async () =>
    {
        await f();
    });
});
*/
function f()
{
    return new Promise((resolve) =>
    {
        const child = spawn("./test/build/test-regexp", ["--greedy"]);
        child.stdin.write("abc*\0");
        child.stdin.write("abc\0");
        child.stdin.end();
        child.stdout.on("data", (data) =>
        {
            console.log(data.toString());
            resolve();
        });
    });
}