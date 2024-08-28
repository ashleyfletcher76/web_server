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
}

window.onload = setupButtons;