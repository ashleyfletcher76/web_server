from locust import HttpUser, task, between

class MyUser(HttpUser):
    wait_time = between(1, 3)

    @task
    def get_homepage(self):
        self.client.get("/")
    
    @task
    def submit_form(self):
        self.client.post("/", data={
            "name": "John Doe",
            "email": "asd@asd",
            "phone": "12345",
            "description": "This is a rest."
        })