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
class SensorMessage(ndb.Model):
    msgType = ndb.StringProperty(repeated=True)
    time = ndb.DateTimeProperty()

class Stats(ndb.Model):
    lastAlive = ndb.DateTimeProperty()