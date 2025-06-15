package backend.tools.utilModels;

import backend.models.User;


public class LoginResponse {
    private LoginStatus status;

    private User user;

    private String token;

    public LoginResponse(LoginStatus status, User user) {
        this.status = status;
        this.user = user;
        this.token = user.getToken();
    }

    public LoginStatus getStatus() {
        return status;
    }

    public void setStatus(LoginStatus status) {
        this.status = status;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
    }
}
