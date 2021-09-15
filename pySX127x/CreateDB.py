import sqlite3
conn = sqlite3.connect('sensordata.db')

c = conn.cursor()

# Create table
c.execute('''CREATE TABLE RXdata (ID, Lat, Lng, Month, Day, Year, Hour, Minute, Seconds, Moisture, Temperature)''')

# Insert a row of data
#c.execute("INSERT INTO data VALUES (1,2,3,4,5,6,7,8,9,10,11)")

# Save (commit) the changes
conn.commit()

# We can also close the connection if we are done with it.
# Just be sure any changes have been committed or they will be lost.
conn.close()
