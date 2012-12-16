from google.appengine.ext import ndb

#class Post(ndb.Model):
#  author = ndb.StringProperty()
#  time = ndb.DateTimeProperty(auto_now_add=True)
#  title = ndb.StringProperty()
#  description = ndb.TextProperty(default='')
#  categories = ndb.StringProperty(repeated=True)
#  last_modified = ndb.DateTimeProperty(auto_now=True)
#
#  @property
#  def agoTime(self):
#    return agoTime(self.time)
class SensorEvent(ndb.Model):
    msgType = ndb.StringProperty()
    time = ndb.DateTimeProperty()

class Alert(ndb.Model):
	startTime = ndb.DateTimeProperty()
	endTime = ndb.DateTimeProperty()
	duration = ndb.IntegerProperty()

class Stats(ndb.Model):
    lastAlive = ndb.DateTimeProperty()
    currentState = ndb.StringProperty()