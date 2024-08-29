function deleteProfile(profileId) {
	fetch('/deleteProfile', {
		method: 'DELETE',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify({ id: profileId })
	})
	.then(response => {
		if (response.ok) {
			return response.text(); // Or response.json() if you know the server returns JSON
		} else {
			throw new Error('Failed to delete profile');
		}
	})
	.then(data => {
		console.log('Response data:', data); // Log the data to see what is returned
		alert('Profile deleted successfully');
		window.location.reload(); // Refresh the page
	})
	.catch((error) => {
		console.error('Error:', error);
		alert('Failed to delete profile');
	});
}
