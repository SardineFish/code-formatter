const exec = require("child_process").exec;
const promisify = require("util").promisify;

module.exports.test = async function(path, args)
{
    const { stdout } = await promisify(exec)([`"${path}"`, ...args.map(arg=>`"${arg}"`)].join(" "));
    if (/null(\r\n|\r|\n)/.test(stdout))
        return null;
    return /(.*)(\r\n)/s.exec(stdout)[1];
}

module.exports.asyncForEach = async function (array, callback)
{
    for (let i = 0; i < array.length; i++)
    {
        await callback(array[i], i);
    }    
}
