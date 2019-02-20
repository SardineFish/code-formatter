const { exec , spawn} = require("child_process");
const promisify = require("util").promisify;

/**
 * @param {string} path
 * @param {string[]} args
 * @param {string} input
 */
module.exports.test = function(path, args, input)
{
    return new Promise((resolve, reject) =>
    {
        console.log(args);
        const child = spawn(`"${path}"`, args);
        if (input)
            child.stdin.write(input);
        child.stdin.end();
        child.stdout.on("data", (data) => {
            resolve(data.toString());
        });
    });
    /*
    //console.log([`"${path}"`, ...args.map(arg => `"${arg.replace(/"/g,'\\"').replace(/\\/g,'\\\\')}"`)].join(" "));
    child.on("exit", (code, signal) => console.log(`${code}: ${signal}`));
    //child.stdin.pipe(process.stdin);
    //child.stdin.write("abc*");
    //child.stdin.write("abccc");
    const { stdout } = await promisify(exec)([`"${path}"`, ...args.map(arg => `"${arg.replace(/"/g, '\\"').replace(/\\\\"/g, '\\\\\\"')}"`)].join(" "));
    if (/null(\r\n|\r|\n)/.test(stdout))
        return null;
    return /(.*)(\r\n)/s.exec(stdout)[1];*/
}

module.exports.asyncForEach = async function (array, callback)
{
    for (let i = 0; i < array.length; i++)
    {
        await callback(array[i], i);
    }    
}
