from datetime import datetime
from dateutil import tz
HERE = tz.gettz('America/New_York')
UTC = tz.gettz('UTC')

def convert_utc_to_local(time):
    gmt = time.replace(tzinfo=UTC)
    local = gmt.astimezone(HERE)
    return local.strftime('%a, %d %b %Y %H:%M:%S')
