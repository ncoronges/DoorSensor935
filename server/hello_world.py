import os
import webapp2
import models
import logging
import json
import datetime

from base_handler import BaseHandler

class HomeHandler(BaseHandler):
  def get(self):
    self.template_out('templates/home.html', template_values={
      'hello_world': 'Door Sensor 935'
    })

class RobotsTextHandler(BaseHandler):
  def get(self):
    allow = os.environ['HTTP_HOST'] == 'doorsensor935.appspot.com' # TODO: Change this after copying boilerplate
    self.template_out('html/robots.txt', {'allow': allow})

class ServiceHandler(BaseHandler):
  def get(self, msgType):
    self.post(msgType)

  def post(self, msgType):
    logging.info("post request: "+ msgType)
    #postdict = json.loads(self.request.body)
    #time = postdict["time"]
    self.response.headers['Content-Type'] = 'application/json'
    
    if msgType == 'alive':
      stats = models.Stats.query().fetch(1)
      logging.info(stats)
      if (len(stats)==0):
        stat = models.Stats(lastAlive=datetime.datetime.now())
        stat.put()
      else:
        stat = stats[0]
        stat.lastAlive = datetime.datetime.now()
        stat.put()

app = webapp2.WSGIApplication([
  webapp2.Route('/service/<msgType>', handler=ServiceHandler,name="services"),
  webapp2.Route('/', handler=HomeHandler,name="home")
], debug=os.environ['SERVER_SOFTWARE'].startswith('Dev'))