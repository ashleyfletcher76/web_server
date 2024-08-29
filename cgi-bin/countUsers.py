import sqlite3
import sys

def count_users(db_path):
	"""Connect to the SQLite database and count the number of users."""
	try:
		conn = sqlite3.connect(db_path)
		cur = conn.cursor()
		cur.execute('SELECT COUNT(*) FROM Users')
		count = cur.fetchone()[0]
		cur.close()
		conn.close()
		return count
	except Exception as e:
		sys.stderr.write("Error accessing database: %s\n" % str(e))
		return None

def main():
	db_path = "./uploads/profiles.userDB.db"
	user_count = count_users(db_path)
	if user_count is not None:
		print ("Content-Type: text/html")
		print
		print ("<html><head><title>User Count</title></head><body>")
		print ("<h1>Total Users: %d</h1>" % user_count)
		print ("</body></html>")

if __name__ == "__main__":
	main()
