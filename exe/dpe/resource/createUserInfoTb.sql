DROP TABLE IF EXISTS user_info;

CREATE TABLE user_info (
    imsi        VARCHAR (16) PRIMARY KEY,
    login_count INTEGER      NOT NULL
                             DEFAULT (1),
    name        VARCHAR (64) 
);