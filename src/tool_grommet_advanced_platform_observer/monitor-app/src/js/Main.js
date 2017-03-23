import React, { Component } from 'react';
import App from 'grommet/components/App';
import Box from 'grommet/components/Box';
import Anchor from 'grommet/components/Anchor';
import Header from 'grommet/components/Header';
import Footer from 'grommet/components/Footer';
import Title from 'grommet/components/Title';
import TodoAppDashboard from './components/TodoAppDashboard';
import ChooseFilesPath from './components/ChooseFilesPath';
import ControlPanel from './components/ControlPanel';
import Monitor from './components/Monitor';

export default class Main extends Component {

  constructor () {
    super();
    this._choosePathToFiles = this._choosePathToFiles.bind(this);
    this._openWindowEditPath = this._openWindowEditPath.bind(this);

    this.state = {
      pathToFiles: "../data ",
      editPathWindowOpen: false
    }
  }



  _choosePathToFiles(choosedPath) {
    this.setState({pathToFiles: choosedPath});
    this.setState({editPathWindowOpen: false});
  }

  _openWindowEditPath(){
    this.setState({editPathWindowOpen: true});
  }

  render () {

    console.log("render runned in Main  %o", this.state)


    return (
      <App centered={false}>
        <Box full={true}>
          <ControlPanel
            pathToFiles={this.state.pathToFiles}
            onClickEditPath={this._openWindowEditPath}/>
          <Box
            pad="small">
            <Title>Monitoring Application</Title>

            <ChooseFilesPath onSubmit={this._choosePathToFiles} hideWindow={!this.state.editPathWindowOpen} defaultText={this.state.pathToFiles} />

            <Monitor nb_socket={2} nb_core={32} />
          </Box>
        </Box>
      </App>
    );
  }
};
