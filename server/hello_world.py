import os
import webapp2
import models
import logging
import json
import datetime
import time
import date_util
from base_handler import BaseHandler
from google.appengine.api import mail

class HomeHandler(BaseHandler):
  def get(self):
    timeString = ""
    currentState = "CLOSED (inferred)"
    lastEvent = ""
    currentTime = datetime.datetime.now()
    events = models.SensorEvent.query().order(-models.SensorEvent.time).fetch(1)
    if (len(events)>0):
      date_to_format = events[0].time
      timeString = date_util.convert_utc_to_local(date_to_format)
      lastEvent = events[0].msgType
      if (lastEvent == "door_opened_alert"):
        elapsed = currentTime - events[0].time
        if (elapsed <= datetime.timedelta(minutes=5)):
          currentState = "OPEN"
    
    # events = models.SensorEvent.query(models.SensorEvent.msgType=='door_opened' or models.SensorEvent.msgType=='door_opened_alert').order(-models.SensorEvent.time).fetch(1)
    # if (len(events)>0):
    #   date_to_format = events[0].time
    #   timeString = date_util.convert_utc_to_local(date_to_format)
    
    # stats = models.Stats.query().fetch(1)
    # if(len(stats)>0):
    #   stat = stats[0]
    #   currentState = stat.currentState

    self.template_out('templates/home.html', template_values={
      'hello_world': 'Door Sensor 935',
      'event_time': timeString,
      'current_state': currentState,
      'last_event': lastEvent
    })

class RobotsTextHandler(BaseHandler):
  def get(self):
    allow = os.environ['HTTP_HOST'] == 'doorsensor935.appspot.com'
    self.template_out('html/robots.txt', {'allow': allow})

# Handles http / json services
class ServiceHandler(BaseHandler):
  def get(self, msgType):
    self.post(msgType)

  def post(self, msgType):
    logging.debug("post request: "+ msgType)
    #postdict = json.loads(self.request.body)
    #time = postdict["time"]
    try:
    
      if msgType == 'alive':
        stats = models.Stats.query().fetch(1)
        logging.info(stats)
        stat=None
        if (len(stats)==0):
          stat = models.Stats()
        else:
          stat = stats[0]
        stat.lastAlive=datetime.datetime.now()
        stat.put()

      elif msgType == 'door_opened_alert':
        self.addRegularSensorEvent(msgType)
        self.updateState(msgType)
        self.sendDoorOpenedAlertMessage()

      elif msgType == 'door_opened':
        self.addRegularSensorEvent(msgType)
        self.updateState(msgType)

      elif msgType == 'door_closed':
        self.addRegularSensorEvent(msgType)
        self.updateState(msgType)

      else:
        self.addRegularSensorEvent(msgType)
      
      self.sendResponse()

    except Exception as e:
      logging.error(e)
      self.sendResponse(False, "error occured")

  def updateState(self, msgType):
      stats = models.Stats.query().fetch(1)
      stat=None
      if (len(stats)==0):
        stat = models.Stats()
      else:
        stat = stats[0]
      stat.currentState = msgType;
      stat.put()

  def sendDoorOpenedAlertMessage(self):
    events = models.SensorEvent.query(models.SensorEvent.msgType=='door_opened').order(-models.SensorEvent.time).fetch(1)
    # TODO: need to find the first door_opened event 

    date_to_format = events[0].time
    timeString = date_util.convert_utc_to_local(date_to_format)
    
    message = mail.EmailMessage(sender="935DoorSensor <notifier@doorsensor935.appspotmail.com>",
                            subject="Door is Opened!")
    message.to = "935pacificalerts@googlegroups.com"
    message.body = """
    Dear Residents:
    The door has been opened since %s
    Shut the door!
    """ %timeString

    message.send()

  def addRegularSensorEvent(self, p_msgType):
    sensorEvent = models.SensorEvent(msgType=p_msgType, time=datetime.datetime.now())
    sensorEvent.put()

  def sendResponse(self, success=True, msg=None):
    self.response.headers['Content-Type'] = 'application/json'
    resp = {'success':success}
    if msg: resp['msg'] = msg
    self.response.out.write(json.dumps(resp))

app = webapp2.WSGIApplication([
  webapp2.Route('/service/<msgType>', handler=ServiceHandler,name="services"),
  webapp2.Route('/', handler=HomeHandler,name="home")
], debug=os.environ['SERVER_SOFTWARE'].startswith('Dev'))