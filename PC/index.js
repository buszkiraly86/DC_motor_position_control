const express = require('express')
const app = express()
const port = 3000
const SerialPort = require("serialport");
const Readline = require('@serialport/parser-readline')
const ByteLength = require('@serialport/parser-byte-length')
const serialPort = new SerialPort('COM7', {
  baudRate: 115200
});

let receivedMsg;

const parser = new Readline()
serialPort.pipe(parser);
parser.on('data', data => {
	receivedMsg = data;
});
serialPort.on('error', function(err) {
  console.log('Error: ', err.message)
})

async function wait(time) {
	return new Promise((resolve, reject)=>{
		setTimeout(resolve, time);
	});
}

async function executeCommand(command) {
	receivedMsg = "";
	
	return new Promise((resolve, reject) => {
		serialPort.write(`${command}\n`, function(err) {
			if (err) return reject(err);
			
			let trys = 10;
			const id = setInterval(() => {
				if (trys-- < 0) {
					clearInterval(id);
					return reject();
				}
				
				if (receivedMsg.length !== 0) {
					clearInterval(id);
					return resolve(receivedMsg);
				}
			}, 100);
		});
	});
}

app.use(express.static('public'));
app.get('/api/:command', async (req, res) => {
	let command = req.params.command;
	if (req.query.parameter) command += " " + req.query.parameter;
	res.send(await executeCommand(command));
});

app.listen(port, () => console.log("The positioning has begun..."))
