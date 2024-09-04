import Navbar from '../Components/Navbar'
import HeroSection from '../Components/HeroSection'
import Jokes from '../Components/Jokes'
import Info from '../Components/Info'
import Footer from '../Components/Footer'
import Intro from '../Components/Intro'
import RandomJoke from '../Components/RandomJoke'
import Speech from '../Components/Speech'
// import Feedback from '../Components/Feedback'/
import FeedbackForm from '../Components/FeedbackForm'
import Thanks from '../Components/Thanks'

const Home = () => {
	return (
		<div>
			<Navbar />
			<HeroSection />
			<Intro />
			<RandomJoke />
			<Speech />
			<Jokes />
			{/* <Feedback /> */}
			<FeedbackForm />
			<Thanks />
			<Footer />
		</div>
	)
}

export default Home
