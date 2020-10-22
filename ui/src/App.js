import React from 'react';
import Axios from 'axios';
import MQTT from 'async-mqtt';
import { v4 as uuidv4 } from 'uuid';
import Moment from 'react-moment';

import './App.scss';

import Menu from './Components/Menu';
import ContentView from './Components/ContentView';

import Home from './Views/Home';
import Devices from './Views/Devices';
import Routines from './Views/Routines';
import Location from './Views/Location';
import Environment from './Views/Environment';

import { faColumns, faPhoneLaptop, faBells, faMapMarker, faTemperatureHot } from '@fortawesome/pro-light-svg-icons';

class App extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            time: new Date(),
            pages: [],
            page: 0,
            notifications: [],
            devices: [],
            data: {
                battery: '-',
                power: '-',
                water: '-',
                network: !!window.navigator.onLine,
                temperatureInside: '-',
                temperatureOutside: '-',
                location: { latitude: 0, longitude: 0, accuracy: 10, speed: 20 },
                weather: undefined,
            },
            locations: [],
            temperatures: [],
        };

        this.setDeviceState = this.setDeviceState.bind(this);
    }

    async setDeviceState(id, key, value) {
        const topic = 'device/' + id + '/' + key;
        console.log(topic, value);
        this.mqtt.publish(topic, JSON.stringify(value), { qos: 2 });
    }

    getWeather() {
        Axios.get(
            'https://api.openweathermap.org/data/2.5/onecall?lat='
            + this.state.data.location.latitude
            + '&lon='
            + this.state.data.location.longitude
            + '&units=metric&exclude=minutely,hourly&appid='
            + process.env.REACT_APP_OPEN_WEATHER_MAP_API_KEY
        )
            .then(response => this.setState({ data: { ...this.state.data, weather: response.data } }))
            .catch(e => {
                this.setState({ data: { ...this.state.data, weather: undefined } });
                this.addNotification('danger', 'Weather request failed: ' + e.message);
            });
    }

    addNotification(type, message) {
        this.setState(
            {
                notifications: [
                    ...this.state.notifications,
                    { id: uuidv4(), type: type, read: false, message: message, created: Date.now() }
                ]
            }
        );
    }

    readNotification(id) {
        let notifications = this.state.notifications.map(n => {
            if (n.id === id)
                n.read = true;
            return n;
        });
        this.setState({ notifications: notifications });
    }

    calculateDistance(lat1, lon1, lat2, lon2) {
        var R = 6371; // Radius of the earth in km
        var dLat = this.deg2rad(lat2 - lat1);  // deg2rad below
        var dLon = this.deg2rad(lon2 - lon1);
        var a =
            Math.sin(dLat / 2) * Math.sin(dLat / 2) +
            Math.cos(this.deg2rad(lat1)) * Math.cos(this.deg2rad(lat2)) *
            Math.sin(dLon / 2) * Math.sin(dLon / 2)
            ;
        var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
        var d = R * c; // Distance in km
        return d;
    }

    deg2rad(deg) {
        return deg * (Math.PI / 180)
    }

    async componentDidMount() {
        window.addEventListener('offline', () => this.setState({ data: { ...this.state.data, network: false } }));
        window.addEventListener('online', () => this.setState({ data: { ...this.state.data, network: true } }));

        this.setState({
            pages: [
                {
                    title: 'Home',
                    icon: faColumns,
                    element: Home,
                },
                {
                    title: 'Devices',
                    icon: faPhoneLaptop,
                    element: Devices,
                },
                /*{
                    title: 'Routines',
                    icon: faBells,
                    element: Routines,
                },*/
                {
                    title: 'Location',
                    icon: faMapMarker,
                    element: Location,
                },
                {
                    title: 'Environment',
                    icon: faTemperatureHot,
                    element: Environment,
                },
            ],
        })

        this.timeInterval = setInterval(() => this.setState({ time: new Date() }), 500);

        this.getWeather();
        this.weatherInterval = setInterval(() => this.getWeather(), 1000 * 60 * 15);

        this.mqtt = await MQTT.connectAsync(process.env.REACT_APP_MQTT_BROKER_URL).catch(error => console.log(error));
        this.mqtt.publish('introduce', '', { qos: 2 });
        this.mqtt.subscribe([
            'device',
            'device/state',
            'data/location',
            'data/temperature',
            'data/battery',
            'data/water',
            'data/network'
        ]);
        this.mqtt.on('message', (topic, message, packet) => {
            let data = JSON.parse(new TextDecoder("utf-8").decode(message));
            console.log('Recieved message', topic, data);
            switch (topic) {
                case 'device': {
                    let devices = [...this.state.devices.filter(d => d.id !== data.id), data].sort((a, b) => {
                        if (a.name < b.name)
                            return -1;
                        if (a.name > b.name)
                            return 1;
                        return 0;
                    });
                    this.setState({ devices: devices });
                    break;
                }
                case 'device/state': {
                    let devices = this.state.devices.map(d => {
                        if (d.id === data.id)
                            d.state = data.state;
                        return d;
                    });
                    this.setState({ devices: devices });
                    break;
                }
                case 'data/location': {
                    const updateWeather = Math.abs(this.state.data.location.latitude) < Number.EPSILON && Math.abs(this.state.data.location.longitude) < Number.EPSILON;
                    const locations = this.state.locations;
                    locations.push([data.latitude, data.longitude, Date.now()]);
                    const date = Date.now() - (1000 * 60 * 60 * 12);
                    this.setState({ data: { ...this.state.data, location: data }, locations: locations.filter(x => x[2] > date) });
                    if (updateWeather)
                        this.getWeather();
                    break;
                }
                case 'data/temperature': {
                    const temperatures = this.state.temperatures;
                    temperatures.push([data.inside, data.outside, Date.now()]);
                    const date = Date.now() - (1000 * 60 * 60 * 12);
                    this.setState({ data: { ...this.state.data, temperatureInside: data.inside, temperatureOutside: data.outside }, temperatures: temperatures.filter(x => x[2] > date) });
                    break;
                }
                case 'data/battery': {
                    this.setState({ data: { ...this.state.data, battery: parseFloat(data.soc) / 10, power: data.power } })
                    break;
                }
                case 'data/water': {
                    this.setState({ data: { ...this.state.data, water: data } });
                    break;
                }
                default:
                    break;
            }
        });
    }

    render() {
        const { pages, page, notifications, devices, data, time, locations, temperatures } = this.state;
        if (pages.length === 0)
            return null;
        let Component = pages[page].element;
        let pageElement = <Component
            devices={devices}
            setDeviceState={(id, key, state) => this.setDeviceState(id, key, state)}
            data={data}
            notifications={notifications}
            locations={locations}
            temperatures={temperatures}
            onAddNotification={(type, message) => this.addNotification(type, message)}
            onReadNotification={id => this.readNotification(id)}
        />;
        return (
            <div className='app'>
                <Moment date={time} className='time' format='dddd Do MMMM YYYY - HH:mm:ss' />
                <Menu pages={pages} selected={page} onPageClick={index => this.setState({ page: index })} />
                <ContentView>
                    {pageElement}
                </ContentView>
            </div>
        );
    }
};

export default App;