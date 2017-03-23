import React, { Component, PropTypes } from 'react';
import Box from 'grommet/components/Box';
import Heading from 'grommet/components/Heading';
import List from 'grommet/components/List';
import ListItem from 'grommet/components/ListItem';
import Meter from 'grommet/components/Meter';
import Value from 'grommet/components/Value';
import Header from 'grommet/components/Header';
import Status from 'grommet/components/icons/Status';
import QPI from './QPI.js';
import Proc from './Proc.js';
import MemBW from './MemBW.js';



export default class Monitor extends Component {

  constructor () {
    super();
    this.state = {
      nb_core: 16
    };
  }

  render () {

    // Creating the sockets
    var html_sockets = [];
    var top_box = [];       // MemBW will be on the top of Monitor
    var bottom_box = [];    // The Procs and the QPI will be on the bottom of Monitor
    for (var i = 0; i < this.props.nb_socket; i++) {
      top_box.push(
        <Box>
          <MemBW numero={i}/>
        </Box>
      );

      if(i==1) { // => There is 2 sockets
        bottom_box.push(
          <QPI />
        );
      }
      bottom_box.push(
        <Proc  nb_core={this.state.nb_core/this.props.nb_socket} numero={i}/>
      );
    }

    return (
      <Box primary={true}  direction='column' id="Monitoring-screen">
        <Box direction="row" colorIndex="accent-1" full="horizontal" style={{height: "30%"}} >
          {top_box}
        </Box>
        <Box direction="row" basis="full"  colorIndex="accent-2">
          {bottom_box}
        </Box>
      </Box>
    );
  }
};


Monitor.propTypes = {
  nb_socket: PropTypes.number.isRequired, // 1 or 2
  nb_core: PropTypes.number.isRequired    // sum of 2 sockets
}
