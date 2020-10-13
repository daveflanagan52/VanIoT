import React from 'react';
import ReactDOM from 'react-dom';

import App from './App';

window._ = require('lodash');
window.Popper = require('popper.js');
window.$ = window.jQuery = require('jquery');
require('bootstrap');

ReactDOM.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
  document.getElementById('root')
);
