const express = require("express");
const path = require("path");
const userUtils = require('./userUtils.js')

const app = express();
app.set("view engine", "hbs");
app.set('views', path.join(__dirname, '/view'));
app.use("/", express.static(path.join(__dirname, '/main')));

app.use("/rights", function (request, response) {
    const username = request.query.username
    const rights = userUtils.getUserRights(username).join(", ")

    response.render("result.hbs", {
        username,
        rights
    });
});
app.listen(3000);
