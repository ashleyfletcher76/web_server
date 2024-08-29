function fetchCgi(scriptPath) {
	fetch(scriptPath)
	.then(response => response.text())
	.then(data => {
		document.getElementById('cgiOutput').innerHTML = data;
	})
	.catch(error => {
		console.error('Error fetching the CGI script:', error);
		document.getElementById('cgiOutput').innerText = "Failed to load CGI script";
	});
}

function fetchUserCount() {
	fetch('/cgi-bin/countUsers.py', {
		method: 'POST',
		headers: {
			'Content-Type': 'application/x-www-form-urlencoded'
		}
	})
	.then(response => {
		if (!response.ok) {
			throw new Error('Network response was not ok');
		}
		return response.text();
	})
	.then(data => {
		console.log('Received data:', data);  // Log data to see what is actually received
		document.getElementById('userCountOutput').innerHTML = data;
	})
	.catch(error => {
		console.error('Error fetching the user count:', error);
		document.getElementById('userCountOutput').innerText = 'Failed to fetch user count: ' + error.message;
	});
}


function setupButtons() {
	document.getElementById('runShellButton').addEventListener('click', function() {
		fetchCgi('/cgi-bin/test.sh');
	}); 

	document.getElementById('runPythonButton').addEventListener('click', function() {
		fetchCgi('/cgi-bin/test.py');
	}); 

	document.getElementById('runPhpButton').addEventListener('click', function() {
		fetchCgi('/cgi-bin/test.php');
	}); 

	document.getElementById('runCppButton').addEventListener('click', function() {
		fetchCgi('/cgi-bin/test.cpp');
	});	

	document.getElementById('countUsersButton').addEventListener('click', function() {
		fetchUserCount();
	});

}

window.onload = setupButtons;
