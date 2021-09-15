from pyowm.owm import OWM
from pyowm.utils import timestamps
from geopy.geocoders import Nominatim
import sqlite3

conn = sqlite3.connect('sensordata.db')
c = conn.cursor()

APIkey="7f791e54ef89c15ea078d2c2c01ad2a6"

geolocator = Nominatim(user_agent="geoapiExercises")

c.execute("SELECT * FROM RXdata ORDER BY rowid DESC LIMIT 1")
result = c.fetchone()
print (result)
Lat = result[1]
Lon = result[2]
location = geolocator.reverse(Lat+","+Lon)
  
# Display
#print(location.raw)
address = location.raw['address']
city = address.get('town', '')
country = address.get('country', '')

#print(city+","+country)

owm = OWM(APIkey)
mgr = owm.weather_manager()
three_h_forecaster = mgr.forecast_at_place(city+","+country, '3h')
tomorrow = timestamps.tomorrow() # datetime object for tomorrow 
rain=three_h_forecaster.will_be_rainy_at(tomorrow)
#print(three_h_forecaster.will_be_rainy_at(tomorrow)) 
#print(rain,result[9])

if ((int(result[9]) < 30) and (rain== True)):
    print ("Area with node ID {} at {} has soil moisture of {} and temperature of {}C with rain expected, requieres medium irrigation".format(result[0],city+","+country,result[9],result[10]))
elif ((int(result[9]) < 30) and (rain== False)):
    print ("Area with node ID {} at {} has soil moisture of {} and temperature of {}C with no rain expected, requieres heavy irrigation".format(result[0],city+","+country,result[9],result[10]))
elif ((int(result[9]) > 30) and (rain== True)):
    print ("Area with node ID {} at {} has soil moisture of {} and temperature of {}C with rain expected, doesnt requiere irrigation".format(result[0],city+","+country,result[9],result[10])) 
else:
    print ("Area with node ID {} at {} has soil moisture of {} and temperature of {}C with no rain expected, requires small irrigation".format(result[0],city+","+country,result[9],result[10]))