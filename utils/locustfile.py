from locust import HttpUser, task, between

class MyUser(HttpUser):
    wait_time = between(1, 3)

    @task
    def get_homepage(self):
        self.client.get("/empty.html", headers={"Connection": "keep-alive"})

    